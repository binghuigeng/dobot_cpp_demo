#include <QCoreApplication>
#include "Handle.h"
#include <QThread>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qDebug() << "main_id is:" << QThread::currentThreadId();

    Handle::getInstance()->initialize();

    int ret = a.exec();
    {
        Handle::deleteInstance();
    }
    return ret;
}
