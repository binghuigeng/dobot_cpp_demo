https://github.com/Dobot-Arm/TCP-IP-Protocol

https://github.com/Dobot-Arm/TCP-IP-CR-CPP

https://github.com/Dobot-Arm/TCP-IP-CR-MATLAB

								
# cpp_demo说明			

程序启动：

1、连接3个服务器端口（Dashboard端口29999、运动相关端口30003、实时反馈端口30004）

2、使能机器人
	下发命令：EnableRobot()，收到反馈：0,{},EnableRobot();
	
3、设置全局速率比
	下发命令：SpeedFactor(50)，收到反馈：0,{},SpeedFactor(50);
	
4、点到点运动，目标点位为关节点位
	下发命令：JointMovJ(120,0,0,0,0,0)，收到反馈：0,{},JointMovJ(120,0,0,0,0,0);
	
5、点到点运动，目标点位为笛卡尔点位
	下发命令：MovJ(-287,652,890,-88,-40,-46)，收到反馈：0,{},MovJ(-287,652,890,-88,-40,-46);
	
6、基于笛卡尔空间的动态跟随命令
	下发命令：ServoP(-287,652,890,-88,-40,-46)


程序退出：

1、下使能机器人
	下发命令：DisableRobot()，收到反馈：0,{},DisableRobot();

2、断开连接的3个服务器端口（Dashboard端口29999、运动相关端口30003、实时反馈端口30004）


# TCP-IP CR-CPP
关于CR的Demo，基于TCP/IP协议的C++的封装，Qt实现的控制Demo。

## 关于版本匹配说明
1. 此Demo适用于CR系列的V3.5.2及以上控制器版本。

## 文件以及类的说明
1. api目录包含了CR的相关功能封装的各个类，基于纯c++编写的。
2. api/rapidjson是[腾讯的json](https://github.com/Tencent/rapidjson)解析库。
3. api目录中的类说明：
    - `BitConverter`封装了字节与基础类型的转换。
    - `DescartesPoint`笛卡尔坐标结构体封装。
    - `JointPoint`关节点坐标结构体封装。
    - `FeedbackData`反馈数据结构体封装。
    - `ErrorInfoBean`错误信息封装。
    - `ErrorInfoHelper`错误信息帮助类。
    - `DobotClient`基于tcp通信的接口类，封装了通信的基础业务。
    - `Dashboard`，`DobotMove`，`Feedback`都是派生于DobotClient，实现了具体的机器人基本功能、运动功能和反馈的具体业务。
4. `alarm_controller.json`警告报警配置文件,`alarm_servo.json`伺服报警配置文件。
5. `Demo.h`,`Demo.cpp`,`main.cpp`,`cpp_demo.pro`是基于Qt实现的控制Demo。

##  运行
1. 打开`Qt Creator`，并加载`CppDemo.pro`
2. 编译并运行。
3. 需要将`alarm_controller.json`和`alarm_servo.json`文件放入到执行文件所在目录。


/****** 基于关节空间的动态跟随命令（关节数据1）******/
const double J11 = 120.0000;
const double J12 = 0.0000;
const double J13 = 0.0000;
const double J14 = 0.0000;
const double J15 = 0.0000;
const double J16 = 0.0000;

/****** 基于关节空间的动态跟随命令（关节数据2）******/
const double J21 = 138.9281;
const double J22 = -57.5286;
const double J23 = 30.0330;
const double J24 = 4.1077;
const double J25 = -4.4503;
const double J26 = -16.6714;


/****** 基于笛卡尔空间的动态跟随命令（笛卡尔数据1）******/
const double X1 = 259.7297;
const double Y1 = 154.3976;
const double Z1 = 1177.0796;
const double Rx1 = -89.8896;
const double Ry1 = -0.0001;
const double Rz1 = -59.4572;

/****** 基于笛卡尔空间的动态跟随命令（笛卡尔数据2）******/
const double X1 = -287.0000;
const double Y1 = 652.0000;
const double Z1 = 890.0000;
const double Rx1 = -88.0000;
const double Ry1 = -40.0000;
const double Rz1 = -46.0000;


/********************************************************************
*                ServoP()分支第一次提交到远程仓库
* 先用MovJ()命令移到下面位置，再用ServoP()命令移到笛卡尔数据2的位置
* 备注：此次提交为第二次提交到远程仓库
********************************************************************/
/****** 点到点运动，目标点位为笛卡尔点位 ******/
const double X1 = -130.0000;
const double Y1 = 300.0000;
const double Z1 = 900.0000;
const double Rx1 = -87.0000;
const double Ry1 = 1.0000;
const double Rz1 = 53.0000;