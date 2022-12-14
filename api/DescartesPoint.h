#pragma once
#include <sstream>
#include <string>

namespace Dobot
{
    struct CDescartesPoint
    {
    public:
        /// <summary>
        /// X 轴位置，单位：毫米
        /// </summary>
        double x;

        /// <summary>
        /// Y 轴位置，单位：毫米
        /// </summary>
        double y;

        /// <summary>
        /// Z 轴位置，单位：毫米
        /// </summary>
        double z;

        /// <summary>
        /// Rx 轴位置，单位：度
        /// </summary>
        double rx;

        /// <summary>
        /// Ry 轴位置，单位：度
        /// </summary>
        double ry;

        /// <summary>
        /// Rz 轴位置，单位：度
        /// </summary>
        double rz;

        std::string ToString()
        {
            std::ostringstream oss;
            oss << x << ',' << y << ',' << z << ',' << rx << ',' << ry << ',' << rz;
            return oss.str();
        }
    };

    struct CDescartesPointIO
    {
    public:
        /// <summary>
        /// X 轴位置，单位：毫米
        /// </summary>
        double x;

        /// <summary>
        /// Y 轴位置，单位：毫米
        /// </summary>
        double y;

        /// <summary>
        /// Z 轴位置，单位：毫米
        /// </summary>
        double z;

        /// <summary>
        /// Rx 轴位置，单位：度
        /// </summary>
        double rx;

        /// <summary>
        /// Ry 轴位置，单位：度
        /// </summary>
        double ry;

        /// <summary>
        /// Rz 轴位置，单位：度
        /// </summary>
        double rz;

        /// <summary>
        /// 设置Distance模式，0：Distance为距离百分比；1：Distance为离起始点或目标点的距离
        /// </summary>
        int mode;

        /// <summary>
        /// 运行指定的距离: 若Mode为0，则Distance表示起始点与目标点之间距离的百分比，取值范围：0~100;
        /// 若Distance取值为正，则表示离起始点的距离; 若Distance取值为负，则表示离目标点的距离
        /// </summary>
        int distance;

        /// <summary>
        /// 数字输出索引，取值范围：1~24
        /// </summary>
        int index;

        /// <summary>
        /// 数字输出状态，取值范围：0或1
        /// </summary>
        int status;

        std::string ToString()
        {
            std::ostringstream oss;
            oss << x << ',' << y << ',' << z << ',' << rx << ',' << ry << ',' << rz
                   << ',' << mode << ',' << distance << ',' << index << ',' << status;
            return oss.str();
        }
    };
}
