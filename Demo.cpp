#include "Demo.h"

#include <QTimer>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDateTime>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>
#include <iomanip>

#include "algorithm/Control.h"
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
    Demo::isrun = false;
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
//        //连接3个服务器端口
        Connect();
        msleep(500);

//        //复位，用于清除错误
//        ClearError();
//        msleep(500);
//        //机器人停止
//        ResetRobot();
//        msleep(500);

//        StopDrag();
//        msleep(1000);

//        /****** Dashboard端口29999 ******/
        //使能机器人
        Enable(true);
        sleep(5);
        //设置全局速率比
        ConfirmSpeed();
        msleep(500);

//        //点到点运动，目标点位为笛卡尔点位
//        //机器人末端移动到离插孔很近的位置
//        MovJ();
//        sleep(3);
//        std::cout << "MovJ end" << std::endl;

        // 直线运动，目标点位为笛卡尔点位
        MovL();
        sleep(5);
        std::cout << "MovL end" << std::endl;

        //进入拖拽(在报错状态下，不可进入拖拽)
//        StartDrag();
//        sleep(3);

#if 0
        Mat1x6 serial_data = {};
//        Mat1x3 fd = {0,0,-10};
        Mat1x3 fd = {0,0,-10};
        Mat1x3 ft = {0,0,0};

        getEndActual();
        control_algorithm.ServoP = robot_data;

//        std::cout << "robot  data : " << robot_data << "\n";
        // 传感器数据
        while (control_algorithm.serial_data.try_dequeue(serial_data)) {
            // sensor_data.push_back(serial_data);
            break;
        }
//        std::cout << "serial data : " << serial_data << "\n";
        control_algorithm.init_sensor = serial_data;

        control_algorithm.FilterRobot(robot_data, 8000);
        init_servop = robot_data;

        while (Demo::isrun) {
            clock_gettime(CLOCK_MONOTONIC, &period);

//            clock_gettime(CLOCK_MONOTONIC, &time_start);
            period.tv_nsec += 4 * 1000 * 1000;
            while (period.tv_nsec >= 1000000000) {
                period.tv_nsec -= 1000000000;
                period.tv_sec++;
            }

            // 准备数据
            // 机器人数据 robot_data
//            std::cout << "get robot value\n";
            getEndActual();
//            std::cout << "robot  : " << robot_data << "\n";
            // 传感器数据
            while (control_algorithm.serial_data.try_dequeue(serial_data)) {
                // sensor_data.push_back(serial_data);
                break;
            }
            // 过滤数据
            control_algorithm.FilterSensor(serial_data ,500);
            control_algorithm.FilterRobot(robot_data ,8000);

            // 受力过大时，进入拖动示教状态
            overstressAutoStartDrag(serial_data, 200);

//            std::cout << "filter data\n";
            // impc()
            ft << serial_data[0], serial_data[1], serial_data[2];
            control_algorithm.impC(fd, ft);
//            std::cout << "impC\n";
            // 欧拉
            control_algorithm.Euler2M4d();
//            std::cout << "Euler\n";
            // transpot to ServoP
            control_algorithm.Transport2ServoP(robot_data);
//            std::cout << "transpot\n";
            ServoP();
            // sleep
            clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &period, NULL);

//            clock_gettime(CLOCK_MONOTONIC, &time_end);
//            std::cout << "time : " << (time_end.tv_nsec-time_start.tv_nsec)/1000.0/1000 << "ms\n";

        }
#endif

//        /****** 运动相关端口30003 ******/
//        //点到点运动，目标点位为关节点位
//        JointMovJ();
//        sleep(5);

//        //点到点运动，目标点位为笛卡尔点位
//        MovJ();

//        unsigned int circulate_count = 0;
//        while (circulate_count < 1000) {
//            //基于笛卡尔空间的动态跟随命令
//            ServoP();
//            msleep(4);
//            circulate_count++;
//        }
//        sleep(1);

        //置标志位为假，准备退出循环
        bStop = false;
//        StopDrag();
        //下使能机器人
        Enable(false);
        //断开连接
        Disconnect();
    }
}

void Demo::getEndActual()
{
    std::stringstream buffer;
    buffer << std::left 
           << std::setw(8) << Demo::getInstance()->getToolVectorActual(0) << ","
           << std::setw(8) << Demo::getInstance()->getToolVectorActual(1) << ","
           << std::setw(8) << Demo::getInstance()->getToolVectorActual(2)  << ","
           << std::setw(8) << Demo::getInstance()->getToolVectorActual(3) << ","
           << std::setw(8) << Demo::getInstance()->getToolVectorActual(4)  << ","
           << std::setw(8) << Demo::getInstance()->getToolVectorActual(5) << ","
           ;

    robot_data << getToolVectorActual(0) , getToolVectorActual(1) , getToolVectorActual(2) , getToolVectorActual(3) , getToolVectorActual(4) , getToolVectorActual(5);

    // robot_data = {getToolVectorActual(0),getToolVectorActual(1),getToolVectorActual(2),getToolVectorActual(3),getToolVectorActual(4),getToolVectorActual(5)});
    while (Demo::isrun && logger.dobot_buffer.enqueue(buffer.str())) {
        break;
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

void Demo::ClearError()
{
    PrintLog(QString::asprintf("send to %s:%hu: ClearError()", m_Dashboard.GetIp().c_str(), m_Dashboard.GetPort()));
    std::thread thd([=]{
        std::string ret = m_Dashboard.ClearError();
        PrintLog(QString::asprintf("Receive From %s:%hu: %s", m_Dashboard.GetIp().c_str(),
                                   m_Dashboard.GetPort(), ret.c_str()));
    });
    thd.detach();
}

void Demo::ResetRobot()
{
    PrintLog(QString::asprintf("send to %s:%hu: ResetRobot()", m_Dashboard.GetIp().c_str(), m_Dashboard.GetPort()));
    std::thread thd([=]{
        std::string ret = m_Dashboard.ResetRobot();
        PrintLog(QString::asprintf("Receive From %s:%hu: %s", m_Dashboard.GetIp().c_str(),
                                   m_Dashboard.GetPort(), ret.c_str()));
    });
    thd.detach();
}

void Demo::overstressAutoStartDrag(Mat1x6 SensorValue, int force_threshold)
{
    for (int i = 0; i < 6; i++) {
        SensorValue(i) = SensorValue(i) - init_sensor(i);
        if (SensorValue(i) > force_threshold) {
            //进入拖拽(在报错状态下，不可进入拖拽)
            StartDrag();
            sleep(3);
            break;
        }
    }
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
//    pt.x = -287.0000;
//    pt.y = 652.0000;
//    pt.z = 890.0000;
//    pt.rx = -88.0000;
//    pt.ry = -40.0000;
//    pt.rz = -46.0000;
    
    //新点一（离插孔稍远）
//    pt.x = 540.0000;
//    pt.y = 19.0000;
//    pt.z = 524.0000;
//    pt.rx = -90.0000;
//    pt.ry = 2.0000;
//    pt.rz = -64.0000;

    //点一（离插孔稍远）
//    pt.x = 448.824;
//    pt.y = 178.199;
//    pt.z = 439.214;
//    pt.rx = -90.121;
//    pt.ry = 1.513;
//    pt.rz = -45.001;

//    //点二（离插孔很近）
//    pt.x = 614.742;
//    pt.y = 67.29;
//    pt.z = 423.727;
//    pt.rx = -89.419;
//    pt.ry = 1.547;
//    pt.rz = -62.841;

    //新点二（离插孔很近）
    pt.x = 614.993;
    pt.y = 50.91;
    pt.z = 423.312;
    pt.rx = -88.348;
    pt.ry = 1.457;
    pt.rz = -63.708;

//    //测试MovL()的起始点
//    pt.x = 561.0000;
//    pt.y = 29.0000;
//    pt.z = 566.0000;
//    pt.rx = -92.0000;
//    pt.ry = 2.0000;
//    pt.rz = -64.0000;

//    PrintLog(QString::asprintf("send to %s:%hu: MovJ(%s)", m_DobotMove.GetIp().c_str(),
//                               m_DobotMove.GetPort(),pt.ToString().c_str()));
    std::thread thd([=]{
        std::string ret = m_DobotMove.MovJ(pt);
//        PrintLog(QString::asprintf("Receive From %s:%hu: %s", m_DobotMove.GetIp().c_str(),
//                                   m_DobotMove.GetPort(), ret.c_str()));
    });
    thd.detach();
}

void Demo::MovL()
{
    CDescartesPoint pt;
//    pt.x = -287.0000;
//    pt.y = 652.0000;
//    pt.z = 890.0000;
//    pt.rx = -88.0000;
//    pt.ry = -40.0000;
//    pt.rz = -53.0000;

    //测试MovL()的终点
//    pt.x = 561.0000;
//    pt.y = 40.0000;
//    pt.z = 566.0000;
//    pt.rx = -92.0000;
//    pt.ry = 2.0000;
//    pt.rz = -64.0000;

    //点二（离插孔很近）
//    pt.x = 614.742;
//    pt.y = 67.29;
//    pt.z = 423.727;
//    pt.rx = -89.419;
//    pt.ry = 1.547;
//    pt.rz = -62.841;

    //新点二（离插孔很近）
    pt.x = 614.993;
    pt.y = 50.91;
    pt.z = 423.312;
    pt.rx = -88.348;
    pt.ry = 1.457;
    pt.rz = -63.708;

    //点三（进入插孔）
//    pt.x = 648.96;
//    pt.y = 68.986;
//    pt.z = 418.781;
//    pt.rx = -87.639;
//    pt.ry = 1.382;
//    pt.rz = -62.587;

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
//    static double x = 540.0000;
//    double x, y, z, rx, ry, rz;
//    x = 448.824;
//    y = 178.199;
//    z = 439.214;
//    rx = -90.121;
//    ry = 1.513;
//    rz = -45.001;
//    static bool bfirst = false;
//    if(x >= 560)
//    {
//        if(!bfirst)
//        {
//            x= 560;
//            std::cout << "ServoP end" << std::endl;
//            bfirst = true;
//        }
//    }
//    else
//    {
//        x += 0.04;
//    }

    CDescartesPoint pt;
//    pt.x = -287.0000;
//    pt.y = 652.0000;
//    pt.z = 890.0000;
//    pt.rx = -88.0000;
//    pt.ry = -40.0000;
//    pt.rz = -46.0000;

    //新点一（离插孔稍远）
//    pt.x = x;
//    pt.y = 19.0000;
//    pt.z = 524.0000;
//    pt.rx = -90.0000;
//    pt.ry = 2.0000;
//    pt.rz = -64.0000;

    //点一（离插孔稍远）
//    pt.x = 448.824;
//    pt.y = 178.199;
//    pt.z = 439.214;
//    pt.rx = -90.121;
//    pt.ry = 1.513;
//    pt.rz = -45.001;

    //点二（离插孔很近）
//    pt.x = 614.742;
//    pt.y = 67.29;
//    pt.z = 423.727;
//    pt.rx = -89.419;
//    pt.ry = 1.547;
//    pt.rz = -62.841;

    //点二（离插孔很近）
    auto &servo_p = control_algorithm.ServoP;
    
    pt.x = servo_p(0);
    pt.y = servo_p(1);
    pt.z = servo_p(2);
    pt.rx = servo_p(3);
    pt.ry = servo_p(4);
    pt.rz = servo_p(5);

//    if (fabs(pt.x-init_servop(0))>100) {
//        pt.x = init_servop(0);
//    }
//    if (fabs(pt.y-init_servop(1))>100) {
//        pt.y = init_servop(1);
//    }
//    if (fabs(pt.z-init_servop(2))>100) {
//        pt.z = init_servop(2);
//    }

//    std::cout << "result : " << pt.ToString() << "\n";
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
