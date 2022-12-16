#include "Demo.h"

#include <QTimer>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDateTime>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>

#include "api/ErrorInfoHelper.h"

//静态成员变量初始化
Demo *Demo::m_instance = nullptr;
QMutex Demo::staticMutex;
bool Demo::bStop = true;


Demo::~Demo()
{
    CDobotClient::UinitNet();

    isrun = false;
    //退出线程
    quit();
    wait();
}

Demo *Demo::getInstance()
{
    if(!m_instance)
    {
        QMutexLocker locker(&staticMutex);
        if(nullptr == m_instance)
        {
            m_instance = new Demo();
        }
    }
    return m_instance;
}

void Demo::deleteInstance()
{
    QMutexLocker locker(&staticMutex);
    if(m_instance)
    {
        delete m_instance;
        m_instance = nullptr;
    }
}

void Demo::run()
{
    qDebug("Start Demo,Please observe the movement of the robot");

    while (bStop) {
        //连接3个服务器端口
        Connect();
        msleep(500);

        /****** Dashboard端口29999 ******/
        //使能机器人
        Enable(true);
        sleep(5);
        //设置全局速率比
        ConfirmSpeed();
        msleep(500);

        /****** 运动相关端口30003 ******/
        //点到点运动，目标点位为关节点位
        JointMovJ();
        sleep(5);

        //点到点运动，目标点位为笛卡尔点位
        MovJ();

        unsigned int circulate_count = 0;
        while (circulate_count < 1000) {
            //基于笛卡尔空间的动态跟随命令
            ServoP();
            msleep(4);
            circulate_count++;
        }
        sleep(1);

        //置标志位为假，准备退出循环
        bStop = false;

        //下使能机器人
        Enable(false);
        //断开连接
        Disconnect();
    }
}

double Demo::getToolVectorActual(int index)
{
    return m_Feedback.GetFeedbackData().ToolVectorActual[index];
}

long Demo::getTimeStamp()
{
    return m_Feedback.GetFeedbackData().TimeStamp;
}

void Demo::slotTimeoutReadFeedback()
{
    if (!m_Feedback.IsDataHasRead())
    {
        return;
    }
    m_Feedback.SetDataHasRead(false);
    ShowDataResult();
}

void Demo::Connect()
{
    QString strIp = "192.168.5.1";
    unsigned short iPortDashboard = 29999;
    unsigned short iPortMove = 30003;
    unsigned short iPortFeedback = 30004;

    PrintLog("Connecting...");
    std::thread thd([=]{
        if (!m_Dashboard.Connect(strIp.toStdString(), iPortDashboard))
        {
            PrintLog(QString::asprintf("Connect %s:%hu Fail!!", strIp.toStdString().c_str(), iPortDashboard));
            return;
        }
        if (!m_DobotMove.Connect(strIp.toStdString(), iPortMove))
        {
            PrintLog(QString::asprintf("Connect %s:%hu Fail!!", strIp.toStdString().c_str(), iPortMove));
            return;
        }
        if (!m_Feedback.Connect(strIp.toStdString(), iPortFeedback))
        {
            PrintLog(QString::asprintf("Connect %s:%hu Fail!!", strIp.toStdString().c_str(), iPortFeedback));
            return;
        }
        PrintLog("Connect Success!!!");
    });
    thd.detach();
}

void Demo::Disconnect()
{
    std::thread thd([this]{
        m_Feedback.Disconnect();
        m_DobotMove.Disconnect();
        m_Dashboard.Disconnect();
        PrintLog("Disconnect success!!!");
    });
    thd.detach();
}

void Demo::Enable(bool bEnable)
{
    PrintLog(QString::asprintf("send to %s:%hu: %s()", m_Dashboard.GetIp().c_str(),
                               m_Dashboard.GetPort(), bEnable? "EnableRobot" : "DisableRobot"));
    std::thread thd([=]{
        std::string ret = bEnable ? m_Dashboard.EnableRobot() : m_Dashboard.DisableRobot();
        bool bOk = ret.find("0")==0;
        if (bOk)
        {

        }
        PrintLog(QString::asprintf("Receive From %s:%hu: %s", m_Dashboard.GetIp().c_str(),
                                   m_Dashboard.GetPort(), ret.c_str()));
    });
    thd.detach();
}

void Demo::ConfirmSpeed()
{
    int iValue = 50;
    PrintLog(QString::asprintf("send to %s:%hu: SpeedFactor(%d)", m_Dashboard.GetIp().c_str(), m_Dashboard.GetPort(),iValue));
    std::thread thd([=]{
        std::string ret = m_Dashboard.SpeedFactor(iValue);
        PrintLog(QString::asprintf("Receive From %s:%hu: %s", m_Dashboard.GetIp().c_str(),
                                   m_Dashboard.GetPort(), ret.c_str()));
    });
    thd.detach();
}

void Demo::StartDrag()
{
    PrintLog(QString::asprintf("send to %s:%hu: StartDrag()", m_Dashboard.GetIp().c_str(), m_Dashboard.GetPort()));
    std::thread thd([=]{
        std::string ret = m_Dashboard.StartDrag();
        PrintLog(QString::asprintf("Receive From %s:%hu: %s", m_Dashboard.GetIp().c_str(),
                                   m_Dashboard.GetPort(), ret.c_str()));
    });
    thd.detach();
}

void Demo::StopDrag()
{
    PrintLog(QString::asprintf("send to %s:%hu: StopDrag()", m_Dashboard.GetIp().c_str(), m_Dashboard.GetPort()));
    std::thread thd([=]{
        std::string ret = m_Dashboard.StopDrag();
        PrintLog(QString::asprintf("Receive From %s:%hu: %s", m_Dashboard.GetIp().c_str(),
                                   m_Dashboard.GetPort(), ret.c_str()));
    });
    thd.detach();
}

void Demo::JointMovJ()
{
    CJointPoint pt;
    pt.j1 = 120.0000;
    pt.j2 = 0.0000;
    pt.j3 = 0.0000;
    pt.j4 = 0.0000;
    pt.j5 = 0.0000;
    pt.j6 = 0.0000;

    PrintLog(QString::asprintf("send to %s:%hu: JointMovJ(%s)", m_DobotMove.GetIp().c_str(),
                               m_DobotMove.GetPort(),pt.ToString().c_str()));
    std::thread thd([=]{
        std::string ret = m_DobotMove.JointMovJ(pt);
        PrintLog(QString::asprintf("Receive From %s:%hu: %s", m_DobotMove.GetIp().c_str(),
                                   m_DobotMove.GetPort(), ret.c_str()));
    });
    thd.detach();
}

void Demo::MovJ()
{
    CDescartesPoint pt;
    pt.x = -287.0000;
    pt.y = 652.0000;
    pt.z = 890.0000;
    pt.rx = -88.0000;
    pt.ry = -40.0000;
    pt.rz = -46.0000;

    PrintLog(QString::asprintf("send to %s:%hu: MovJ(%s)", m_DobotMove.GetIp().c_str(),
                               m_DobotMove.GetPort(),pt.ToString().c_str()));
    std::thread thd([=]{
        std::string ret = m_DobotMove.MovJ(pt);
        PrintLog(QString::asprintf("Receive From %s:%hu: %s", m_DobotMove.GetIp().c_str(),
                                   m_DobotMove.GetPort(), ret.c_str()));
    });
    thd.detach();
}

void Demo::MovL()
{
    CDescartesPoint pt;
    pt.x = -287.0000;
    pt.y = 652.0000;
    pt.z = 890.0000;
    pt.rx = -88.0000;
    pt.ry = -40.0000;
    pt.rz = -53.0000;
    PrintLog(QString::asprintf("send to %s:%hu: MovL(%s)", m_DobotMove.GetIp().c_str(),
                               m_DobotMove.GetPort(),pt.ToString().c_str()));
    std::thread thd([=]{
        std::string ret = m_DobotMove.MovL(pt);
        PrintLog(QString::asprintf("Receive From %s:%hu: %s", m_DobotMove.GetIp().c_str(),
                                   m_DobotMove.GetPort(), ret.c_str()));
    });
    thd.detach();
}

void Demo::MovJIO()
{
    CDescartesPointIO pt;
    pt.x = -287.0000;
    pt.y = 652.0000;
    pt.z = 890.0000;
    pt.rx = -88.0000;
    pt.ry = -40.0000;
    pt.rz = -46.0000;
    pt.mode = 0;
    pt.distance = 50;
    pt.index = 1;
    pt.status = 1;
    PrintLog(QString::asprintf("send to %s:%hu: MovJIO(%s)", m_DobotMove.GetIp().c_str(),
                               m_DobotMove.GetPort(),pt.ToString().c_str()));
    std::thread thd([=]{
        std::string ret = m_DobotMove.MovJIO(pt);
        PrintLog(QString::asprintf("Receive From %s:%hu: %s", m_DobotMove.GetIp().c_str(),
                                   m_DobotMove.GetPort(), ret.c_str()));
    });
    thd.detach();
}

void Demo::MovLIO()
{
    CDescartesPointIO pt;
    pt.x = -287.0000;
    pt.y = 652.0000;
    pt.z = 890.0000;
    pt.rx = -88.0000;
    pt.ry = -40.0000;
    pt.rz = -53.0000;
    pt.mode = 0;
    pt.distance = 50;
    pt.index = 1;
    pt.status = 0;
    PrintLog(QString::asprintf("send to %s:%hu: MovLIO(%s)", m_DobotMove.GetIp().c_str(),
                               m_DobotMove.GetPort(),pt.ToString().c_str()));
    std::thread thd([=]{
        std::string ret = m_DobotMove.MovLIO(pt);
        PrintLog(QString::asprintf("Receive From %s:%hu: %s", m_DobotMove.GetIp().c_str(),
                                   m_DobotMove.GetPort(), ret.c_str()));
    });
    thd.detach();
}

void Demo::ServoJ()
{
    CJointPoint pt;
    pt.j1 = 120.0000;
    pt.j2 = 0.0000;
    pt.j3 = 0.0000;
    pt.j4 = 0.0000;
    pt.j5 = 0.0000;
    pt.j6 = 0.0000;

    PrintLog(QString::asprintf("send to %s:%hu: ServoJ(%s)", m_DobotMove.GetIp().c_str(),
                               m_DobotMove.GetPort(),pt.ToString().c_str()));
    std::thread thd([=]{
        std::string ret = m_DobotMove.ServoJ(pt);
        PrintLog(QString::asprintf("Receive From %s:%hu: %s", m_DobotMove.GetIp().c_str(),
                                   m_DobotMove.GetPort(), ret.c_str()));
    });
    thd.detach();
}

void Demo::ServoP()
{
    CDescartesPoint pt;
    pt.x = -287.0000;
    pt.y = 652.0000;
    pt.z = 890.0000;
    pt.rx = -88.0000;
    pt.ry = -40.0000;
    pt.rz = -46.0000;
//    PrintLog(QString::asprintf("send to %s:%hu: ServoP(%s)", m_DobotMove.GetIp().c_str(),
//                               m_DobotMove.GetPort(),pt.ToString().c_str()));
    std::thread thd([=]{
        std::string ret = m_DobotMove.ServoP(pt);
//        PrintLog(QString::asprintf("Receive From %s:%hu: %s", m_DobotMove.GetIp().c_str(),
//                                   m_DobotMove.GetPort(), ret.c_str()));
    });
    thd.detach();
}

void Demo::MoveJog()
{
    DoMoveJog("J1+");
}

void Demo::StopMoveJog()
{
    DoStopMoveJog();
}

void Demo::PrintLog(QString strLog)
{
    emit signalPrintLog(strLog);
}

void Demo::DoMoveJog(QString str)
{
    PrintLog(QString::asprintf("send to %s:%hu: MoveJog(%s)", m_DobotMove.GetIp().c_str(),
                               m_DobotMove.GetPort(),str.toStdString().c_str()));
    std::thread thd([=]{
        std::string ret = m_DobotMove.MoveJog(str.toStdString());
        PrintLog(QString::asprintf("Receive From %s:%hu: %s", m_DobotMove.GetIp().c_str(),
                                   m_DobotMove.GetPort(), ret.c_str()));
    });
    thd.detach();
}

void Demo::DoStopMoveJog()
{
    PrintLog(QString::asprintf("send to %s:%hu: MoveJog()", m_DobotMove.GetIp().c_str(),
                               m_DobotMove.GetPort()));
    std::thread thd([=]{
        std::string ret = m_DobotMove.MoveJog("");
        PrintLog(QString::asprintf("Receive From %s:%hu: %s", m_DobotMove.GetIp().c_str(),
                                   m_DobotMove.GetPort(), ret.c_str()));
    });
    thd.detach();
}

void Demo::ShowDataResult()
{
    ParseWarn();
}

void Demo::ParseWarn()
{
    if (m_Feedback.GetFeedbackData().RobotMode != 9) return;

    //strResult=ErrorID,{[[id,...,id], [id], [id], [id], [id], [id], [id]]},GetErrorID()
    std::string strResult = m_Dashboard.GetErrorID();
    if (strResult.find("0")!=0) return;

    //截取第一个{}内容
    size_t iBegPos = strResult.find('{');
    if (iBegPos == std::string::npos) return;
    size_t iEndPos = strResult.find('}', iBegPos + 1);
    if (iEndPos <= iBegPos) return;
    strResult = strResult.substr(iBegPos + 1, iEndPos - iBegPos - 1);
    if (strResult.empty()) return;

    //剩余7组[]，第1组是控制器报警，其他6组是伺服报警
    QString strShowTxt;
    QJsonDocument doc = QJsonDocument::fromJson(QByteArray(strResult.c_str(), strResult.size()));
    do{
        if (!doc.isArray()) break;
        QJsonArray arrWarn = doc.array();
        for (int i = 0; i < arrWarn.size(); ++i)
        {
            if (!arrWarn[i].isArray()) continue;

            QJsonArray arr = arrWarn[i].toArray();
            for (int j = 0; j < arr.size(); ++j)
            {
                bool bFind = false;
                CErrorInfoBean bean;
                if (0 == i)
                {//控制器报警
                    bFind = CErrorInfoHelper::FindController(arr[j].toInt(), bean);
                }
                else
                {//伺服报警
                    bFind = CErrorInfoHelper::FindServo(arr[j].toInt(), bean);
                }
                if (bFind)
                {
                    strShowTxt.append(QString::asprintf("ID:%d\r\n",bean.id));
                    strShowTxt.append(QString::asprintf("Type:%s\r\n",bean.Type.c_str()));
                    strShowTxt.append(QString::asprintf("Level:%d\r\n",bean.level));
                    strShowTxt.append(QString::asprintf("Solution:%s\r\n",bean.en.solution.c_str()));
                }
            }
        }
    }while(false);

    if (!strShowTxt.isEmpty())
    {
        QString strTime = QString("Time Stamp: ") + QDateTime::currentDateTime().toLocalTime().toString("yyyy-MM-dd HH:mm:ss");
        qDebug() << strTime;
        qDebug() << strShowTxt;
    }
}

Demo::Demo(QObject *parent) : QThread(parent)
{
    CDobotClient::InitNet();
    m_pTimerReader = new QTimer(this);
    m_pTimerReader->setInterval(100);
    connect(m_pTimerReader, &QTimer::timeout, this, &Demo::slotTimeoutReadFeedback);

    connect(this, &Demo::signalPrintLog, this, [](QString str){
        qDebug() << str;
    });

    m_pTimerReader->start();

    QString strPath = qApp->applicationDirPath() + QDir::separator();
    CErrorInfoHelper::ParseControllerJsonFile(QString(strPath+ "alarm_controller.json").toStdString());
    CErrorInfoHelper::ParseServoJsonFile(QString(strPath + "alarm_servo.json").toStdString());
}
