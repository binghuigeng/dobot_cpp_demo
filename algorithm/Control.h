#ifndef CONTROL_H
#define CONTROL_H

#include <eigen3/Eigen/Eigen>
#include "3rd-party/concurrentqueue.h"

using namespace Eigen;
typedef Eigen::Matrix<double, 1, 3> Mat1x3;
typedef Eigen::Matrix<double, 1, 4> Mat1x4;
typedef Eigen::Matrix<double, 1, 6> Mat1x6;
typedef Eigen::Matrix<double, 3, 3> Mat3x3;
typedef Eigen::Matrix<double, 4, 4> Mat4x4;
#define RAD(angleDegrees) (angleDegrees * 3.1415926536 / 180.0)
class Control {
   public:
    ~Control() {  }
    Control(const Control&) = delete;
    Control& operator=(const Control&) = delete;
    static Control& get_instance() {
        static Control instance;
        return instance;
    }

    static Mat1x6 preSensorValue, SensorValue;
    static Mat1x6 preRobotValue, RobotValue;
    static Mat3x3 Kxyz, prePose, expPose;
    static Mat4x4 ToolMatonBase;

    void impC(Mat1x3 fd, Mat1x3 ft, Mat1x3 expPose);
    void Euler2M4d(Mat1x6 ToolVectorActual);
    Mat1x3 integral(Mat1x3 u);
    void Transport2ServoP();
    void FilterSensor(Mat1x6 SensorValue,int filter_value);
    void FilterRobot(Mat1x6 RobotValue,int filter_value);

    moodycamel::ConcurrentQueue<Mat1x6> dobot_data;
    moodycamel::ConcurrentQueue<Mat1x6> serial_data;
   private:
    Control();
    static double eps;
    static Mat1x3 x1;
};
#endif  // CONTROL_H
