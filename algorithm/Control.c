#include "Control.h"

Control::Control() {
    eps = 8e-3;
    // xi << 0, 0, 0;
    Kxyz << 0.5, 40.0, 10.0,  // Kp Kd Ke
        0.5, 40, 10, 0.5, 40, 10;
}

// ToolVectorActual to Mat4x4 (欧拉角转旋转矩阵 x/pitch, y/yaw, z/roll)
void Control::Euler2M4d()  // 从机器人读到的值 输入是mm 角度
{
    ToolVectorActual = preRobotValue;
    ToolMatonBase.block(0, 3, 3, 3) = ToolVectorActual.block(0, 0, 0, 2).transpose();
    ToolMatonBase.block.row(3) << 0, 0, 0, 1;
    // std::cout << "RZ/RY/RX: " << RZ << " " << RY << " " << RX << std::endl;
    // Vector3d eulerAngle(RAD(RZ),RAD(RY),RAD(RX));
    AngleAxisd rollAngle(AngleAxisd(RAD(ToolVectorActual(5)), Vector3d::UnitX()));   // z
    AngleAxisd pitchAngle(AngleAxisd(RAD(ToolVectorActual(4)), Vector3d::UnitY()));  // y
    AngleAxisd yawAngle(AngleAxisd(RAD(ToolVectorActual(3)), Vector3d::UnitZ()));    // x

    ToolMatonBase.block(0, 0, 2, 2) = Matrix3d(yawAngle * pitchAngle * rollAngle);
}

Mat1x3 Control::integral(Mat1x3 u) {
    Mat1x3 x2;
    x2 = eps * u + x1;
    x1 = x2;
    return x2;
}

void Control::impC(Mat1x3 Fd, Mat1x3 Ft)  // for Fd 期望力Fx Fy Fz, Ft是测量力
{
    Mat1x3 m, a, v, s;
    a = prePose.block(0, 0, 0, 2);
    v = prePose.block(1, 0, 1, 2);
    s = prePose.block(2, 0, 2, 2);
    for (int i = 0; i < 3; i++) {
        m(i) = (Fd(i) - Ft(i)) - Kxyz.block(i, 1) * v(i) - Kxyz(i, 2) * s(i);  // kd ke
        a(i) = m(i) / Kxyz(i, 0);                                              // kp
    }
    v = integral(a);
    s = integral(v);
    expPose << a, v, s;
    prePose = expPose;
}
void Control::Transport2ServoP() {
    Mat1x4 expP;
    expP.block(0, 0, 0, 2) = expPose;
    expP(3) = 1;
    // deltaP = ToolMatonBase * P.transpose() - P.transpose();
    // if(deltaP)
    expP = ToolMatonBase * expP.transpose();
    ServoP.block(0, 0, 0, 2) = expP.block(0, 3, 2, 3).transpose();
    ServoP.block(0, 3, 0, 5) = ToolVectorActual.block(0, 3, 0, 5);
}
void Control::FilterSensor(Mat1x6 SensorValue, int filter_value) {
    for (int i = 0; i < 6; i++) {
        if (fabs(SensorValue(i) - preSensorValue(i)) > filter_value) {
            SensorValue(i) = preSensorValue(i);
        }
    }
    preSensorValue = SensorValue;
}

void Control::FilterRobot(Mat1x6 RobotValue, int filter_value) {
    for (int i = 0; i < 6; i++) {
        if (fabs(RobotValue(i) - preRobotValue(i)) > filter_value) {
            RobotValue(i) = preRobotValue(i);
        }
    }
    preRobotValue = RobotValue;
}
