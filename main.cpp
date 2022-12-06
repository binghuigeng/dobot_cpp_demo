#include <QCoreApplication>
#include "Demo.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //启动Demo线程
    Demo::getInstance()->start();

    int ret = a.exec();
    {
        Demo::deleteInstance();
    }
    return ret;
}
