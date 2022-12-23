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
    ~Control() {}
    Control(const Control&) = delete;
    Control& operator=(const Control&) = delete;
    static Control& get_instance() {
        static Control instance;
        return instance;
    }

    Mat1x6 preSensorValue, SensorValue, ServoP,filtered_tool_vector_actual;
    Mat1x6 preRobotValue, RobotValue, init_sensor;
    Mat3x3 Kxyz, prePose, expPose;
    Mat4x4 ToolMatonBase;

    void impC(Mat1x3 fd, Mat1x3 ft);
    void Euler2M4d();
    Mat1x3 integral(Mat1x3 u);
    void Transport2ServoP(Mat1x6 real_value);
    void FilterSensor(Mat1x6 SensorValue, int filter_value);
    void FilterRobot(Mat1x6 RobotValue, int filter_value);

    moodycamel::ConcurrentQueue<Mat1x6> dobot_data;
    moodycamel::ConcurrentQueue<Mat1x6> serial_data;

   private:
    Control();
    double eps = 0;
    Mat1x3 x1;
    double kavs_limit[3];
};
#endif  // CONTROL_H