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
	
5、基于关节空间的动态跟随命令
	下发命令：ServoJ(120,0,0,0,0,0)
	
6、点到点运动，目标点位为笛卡尔点位
	下发命令：MovJ(-287,652,890,-88,-40,-46)，收到反馈：0,{},MovJ(-287,652,890,-88,-40,-46);
	
7、直线运动，目标点位为笛卡尔点位
	下发命令：MovL(-287,652,890,-88,-40,-53)，收到反馈：0,{},MovL(-287,652,890,-88,-40,-53);
	
8、点动
	下发命令：MoveJog(J1+)，收到反馈：0,{},MoveJog(J1+);
	下发命令：MoveJog()，收到反馈：0,{},MoveJog();


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