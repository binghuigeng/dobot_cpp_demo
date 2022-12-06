#ifndef DEMO_H
#define DEMO_H

#include <QObject>
#include <QThread>
#include <QMutex>

#include "api/Dashboard.h"
#include "api/DobotMove.h"
#include "api/Feedback.h"

using namespace Dobot;

class QTimer;

class Demo : public QThread
{
    Q_OBJECT
public:
    ~Demo();
    //单例模式
    static Demo *getInstance();
    static void deleteInstance();
    void run();

signals:
    void signalPrintLog(QString strLog);

private slots:
    void slotTimeoutReadFeedback();

private:
    //连接
    void Connect();
    //断开连接
    void Disconnect();

    //使能/下使能
    void Enable(bool bEnable);

    //设置全局速率比
    void ConfirmSpeed();

    //点到点运动，目标点位为关节点位
    void JointMovJ();

    //点到点运动，目标点位为笛卡尔点位
    void MovJ();

    //直线运动，目标点位为笛卡尔点位
    void MovL();

    //点动
    void MoveJog();
    void StopMoveJog();

    void PrintLog(QString strLog);
    void DoMoveJog(QString str);
    void DoStopMoveJog();
    void ShowDataResult();
    void ParseWarn();

private:
    explicit Demo(QObject *parent = nullptr);
    static Demo *m_instance;
    static QMutex staticMutex;
    static bool bStop;

    QTimer* m_pTimerReader;

    CFeedback m_Feedback;
    CDobotMove m_DobotMove;
    CDashboard m_Dashboard;
};

#endif // DEMO_H
