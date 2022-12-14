#include <QCoreApplication>
#include "Demo.h"
#include "SerialPort.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //启动Demo线程
    Demo::getInstance()->start();

    //启动串口模块
    SerialPort uart;
    uart.start();

    int ret = a.exec();
    {
        Demo::deleteInstance();
    }
    return ret;
}
