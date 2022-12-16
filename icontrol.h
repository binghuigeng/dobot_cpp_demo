#include <Eigen/Dense>


typedef Matrix<double, 1, 3> Mat1x3
typedef Matrix<double, 1, 4> Mat1x4
typedef Matrix<double, 1, 6> Mat1x6
typedef Matrix<double, 3, 3> Mat3x3
typedef Matrix<double, 4, 4> Mat4x4
#define RAD(angleDegrees) (angleDegrees * 3.1415926536 / 180.0)
class Icontrol
{
public:
    Icontrol();
    ~Icontrol();
    static Mat1x6 preSensorValue, SensorValue;
    static Mat3x3 Kxyz, prePose, expPose;
    static Mat4x4 ToolMatonBase;
     impC(Vector3d Kx, Vector3d prePose, Vector3d expPose);
private:
    static double eps;
    static Mat1x3 x1;






}