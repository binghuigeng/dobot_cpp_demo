#pragma once

#include "DobotClient.h"
#include "DescartesPoint.h"
#include "JointPoint.h"

namespace Dobot
{
    class CDobotMove : public CDobotClient
    {
    public:
        CDobotMove();
        virtual ~CDobotMove() override;

        /// <summary>
        /// 关节点动运动，不固定距离运动
        /// </summary>
        /// <param name="s">点动运动轴</param>
        /// <returns>返回执行结果的描述信息</returns>
        std::string MoveJog(std::string s);

        /// <summary>
        /// 停止关节点动运动
        /// </summary>
        /// <returns>返回执行结果的描述信息</returns>
        std::string StopMoveJog();

        /// <summary>
        /// 点到点运动，目标点位为笛卡尔点位
        /// </summary>
        /// <param name="pt">笛卡尔点位</param>
        /// <returns>返回执行结果的描述信息</returns>
        std::string MovJ(const CDescartesPoint& pt);

        /// <summary>
        /// 直线运动，目标点位为笛卡尔点位
        /// </summary>
        /// <param name="pt">笛卡尔点位</param>
        /// <returns>返回执行结果的描述信息</returns>
        std::string MovL(const CDescartesPoint& pt);

        /// <summary>
        /// 点到点运动，目标点位为关节点位。
        /// </summary>
        /// <param name="pt">关节点位</param>
        /// <returns>返回执行结果的描述信息</returns>
        std::string JointMovJ(const CJointPoint& pt);

        /// <summary>
        /// 点到点运动时并行设置数字输出端口状态，目标点位为笛卡尔点位。
        /// </summary>
        /// <param name="pt">笛卡尔点位</param>
        /// <returns>返回执行结果的描述信息</returns>
        std::string MovJIO(const CDescartesPointIO& pt);

        /// <summary>
        /// 在直线运动时并行设置数字输出端口状态，目标点位为笛卡尔点位
        /// </summary>
        /// <param name="pt">笛卡尔点位</param>
        /// <returns>返回执行结果的描述信息</returns>
        std::string MovLIO(const CDescartesPointIO& pt);

        /// <summary>
        /// 基于关节空间的动态跟随命令。
        /// </summary>
        /// <param name="pt">关节空间</param>
        /// <returns>返回执行结果的描述信息</returns>
        std::string ServoJ(const CJointPoint& pt);

        /// <summary>
        /// 基于笛卡尔空间的动态跟随命令。
        /// </summary>
        /// <param name="pt">笛卡尔空间</param>
        /// <returns>返回执行结果的描述信息</returns>
        std::string ServoP(const CDescartesPoint& pt);

    protected:
        void OnConnected() override;
        void OnDisconnected() override;
    };
}
