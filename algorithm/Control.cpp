#include "Control.h"
#include <iostream>
#include <math.h>

#define PI 3.1415926

Control::Control() {
    eps = 4e-3;
  //  eps = 1;
    x1 << 0, 0, 0;
    Kxyz << 0.5, 45.0, 10.0,  // Kp Kd Ke
        0.5, 45, 10.0,
        0.5, 45, 10.0;

    preRobotValue << 0, 0, 0, 0, 0, 0;
    prePose << 0, 0, 0,//
            0, 0, 0,//
            0, 0, 0;
    kavs_limit[0] = 5000;
    kavs_limit[1] = 1000;
//    kavs_limit[2] = 0.08;
    kavs_limit[2] = 0.04;
}

// ToolVectorActual to Mat4x4 (欧拉角转旋转矩阵 x/pitch, y/yaw, z/roll)
void Control::Euler2M4d()  // 从机器人读到的值 输入是mm 角度
{
    filtered_tool_vector_actual = preRobotValue;
    ToolMatonBase.block(0, 3, 3, 1) = filtered_tool_vector_actual.block(0, 0, 1, 3).transpose();
    ToolMatonBase.row(3) << 0, 0, 0, 1;
    // std::cout << "RZ/RY/RX: " << RZ << " " << RY << " " << RX << std::endl;
    // Vector3d eulerAngle(RAD(RZ),RAD(RY),RAD(RX));
    AngleAxisd rollAngle(AngleAxisd(RAD(filtered_tool_vector_actual(5)), Vector3d::UnitX()));   // z
    AngleAxisd pitchAngle(AngleAxisd(RAD(filtered_tool_vector_actual(4)), Vector3d::UnitY()));  // y
    AngleAxisd yawAngle(AngleAxisd(RAD(filtered_tool_vector_actual(3)), Vector3d::UnitZ()));    // x

    ToolMatonBase.block(0, 0, 3, 3) = Matrix3d(yawAngle * pitchAngle * rollAngle);
}

Mat1x3 Control::integral(Mat1x3 u) {
    Mat1x3 x2;
    x2 = eps * u + x1;
    x1 = x2;
    return x2;
}

void Control::impC(Mat1x3 Fd, Mat1x3 Ft)  // for Fd 期望力Fx Fy Fz, Ft是测量力
{
    Mat1x3 m, a, v, s,ds;
    a.block(0, 0, 1, 3) = prePose.block(0, 0, 1, 3);
    v.block(0, 0, 1, 3) = prePose.block(1, 0, 1, 3);
    s.block(0, 0, 1, 3) = prePose.block(2, 0, 1, 3);

    for (int i = 0; i < 3; i++) {
        m(i) = (Ft(i) - Fd(i)) - Kxyz(i, 1) * v(i) - Kxyz(i, 2) * s(i);  // kd ke
        a(i) = m(i) / Kxyz(i, 0);                                              // kp
    }
    v = integral(a);
    s = integral(v);
    ds = s;
    expPose << a, v, ds;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (expPose(i, j) > kavs_limit[i]) {
                expPose(i, j) = kavs_limit[i];
            }
        }
    }
    prePose = expPose;
}
void Control::Transport2ServoP(Mat1x6 real_value) {
    Mat1x4 expP;
//    std::string sep = "\n----------------------------------------\n";
//    IOFormat HeavyFmt(FullPrecision, 0, ", ", ";\n", "[", "]", "[", "]");
//    expP.block(0, 0, 0, 2) = expPose.block();
    
//    expP(0) = expPose(2,2); // x = z
    //    expP(1) = expPose(2,0); // y = x
    //    expP(2) = -1 * expPose(2,1); // z = -y
//    expP(0) = expPose(2,0); // x = x
//    expP(1) = expPose(2,1); // y = y
//    expP(2) = expPose(2,2); // z = z
//    expP(0) = 18/12*expPose(2,2) + 0*expPose(2,1) + 0*expPose(2,0); // x = x
//    expP(1) = 6/12*expPose(2,2); // y = y
//    expP(2) = 10/12*expPose(2,2); // z = z
    double x_de = 649.639-627.08;
    double y_de = 68.489-56.17;
    double z_de = 424.019-424.059;

    double a,b,c;  //获z轴移动向量，并单位化
    a = x_de / sqrt(pow(x_de, 2) + pow(y_de, 2) + pow(z_de, 2));
    b = y_de / sqrt(pow(x_de, 2) + pow(y_de, 2) + pow(z_de, 2));
    c = z_de / sqrt(pow(x_de, 2) + pow(y_de, 2) + pow(z_de, 2));



    //第一列为插孔末端z轴移动向量在base坐标系(a,b,c)，第二列为插孔末端Y轴移动向量在base坐标系(0,0,-1)，
    //第三列为插孔末端x轴移动向量(c,b,-a)

     std::cout << "x轴受力控xy影响 : " <<  0*expPose(2,1) + c*expPose(2,0)<< "\n";
     std::cout << "y轴受力控xy影响 : " <<  0*expPose(2,1) + b*expPose(2,0)<< "\n";
     std::cout << "z轴受力控xy影响 : " <<  -1*expPose(2,1) - a*expPose(2,0)<< "\n";
//     expP(0) = a*expPose(2,2) + 0*expPose(2,1) + c*expPose(2,0); // x = x
//     expP(1) = b*expPose(2,2)+ 0*expPose(2,1) + b*expPose(2,0); // y = y
//     expP(2) = c*expPose(2,2)- 1*expPose(2,1) - a*expPose(2,0); // z = z
    expP(0) = a*expPose(2,2); // x = x
    expP(1) = b*expPose(2,2); // y = y
    expP(2) = c*expPose(2,2); // z = z
    expP(3) = 1;
    // deltaP = ToolMatonBase * P.transpose() - P.transpose();
    // if(deltaP)
//    expP = ToolMatonBase * expP.transpose();

//    std::cout << ToolMatonBase.format(HeavyFmt) << sep;
//    std::cout << expP.format(HeavyFmt) << sep;
//    ServoP.block(0, 0, 1, 3) = expP.block(0, 0, 1, 3); // todo 真实相加
      ServoP.block(0, 0, 1, 3) += expP.block(0, 0, 1, 3); // todo 真实相加
//    ServoP.block(0, 0, 1, 3) += real_value.block(0, 0, 1, 3); // 真实值相加
      ServoP.block(0, 3, 1, 3) = preRobotValue.block(0, 3, 1, 3);
}
void Control::FilterSensor(Mat1x6 SensorValue, int filter_value) {
    for (int i = 0; i < 6; i++) {
        SensorValue(i) = SensorValue(i) - init_sensor(i);
        if (fabs(SensorValue(i) - preSensorValue(i)) > filter_value) {
            SensorValue(i) = preSensorValue(i);
        }
    }
    preSensorValue = SensorValue;
    std::cout << "serial data : " << preSensorValue << "\n";
}

void Control::FilterRobot(Mat1x6 RobotValue, int filter_value) {
    for (int i = 0; i < 6; i++) {
        if (fabs(RobotValue(i) - preRobotValue(i)) > filter_value) {
            RobotValue(i) = preRobotValue(i);
        }
    }
    preRobotValue = RobotValue;
}
