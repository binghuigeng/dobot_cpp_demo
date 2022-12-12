#include "SerialPort.h"
#include <QDebug>

SerialPort::SerialPort(QObject *parent) : QObject(parent)
{
    my_thread = new QThread();

    // 串口类必须在 moveToThread 之前初始化，否则程序会 crash（崩溃）
    port = new QSerialPort();
    init_port();
    this->moveToThread(my_thread);
    port->moveToThread(my_thread);
    my_thread->start();  //启动线程
}

SerialPort::~SerialPort()
{
    port->close();
    port->deleteLater();
    my_thread->quit();
    my_thread->wait();
    my_thread->deleteLater();
}

void SerialPort::init_port()
{
    qDebug() << "init_port_id is:" << QThread::currentThreadId();
#ifdef WIN32
    port->setPortName("COM3");                   //串口名 linux下写作/dev/ttyS1
#else
    port->setPortName("/dev/ttyS1");                   //串口名 windows下写作COM1
#endif
//    port->setBaudRate(QSerialPort::Baud115200);                           //波特率
    port->setBaudRate(460800);                         //波特率
    port->setDataBits(QSerialPort::Data8);             //数据位
    port->setStopBits(QSerialPort::OneStop);           //停止位
    port->setParity(QSerialPort::NoParity);            //奇偶校验
    port->setFlowControl(QSerialPort::NoFlowControl);  //流控制
    if (port->open(QIODevice::ReadWrite))
    {
        qDebug() << "Port have been opened";
    }
    else
    {
        qDebug() << "open it failed";
    }
    connect(port, SIGNAL(readyRead()), this, SLOT(handle_data()), Qt::QueuedConnection); //Qt::DirectConnection
}

void SerialPort::handle_data()
{
    QByteArray buf = port->readAll();
    qDebug() << QStringLiteral("data received(收到的数据):") << buf.toHex().data();
    qDebug() << "handing thread is:" << QThread::currentThreadId();
    emit receive_data(buf);
}

void SerialPort::write_data()
{
    qDebug() << "write_id is:" << QThread::currentThreadId();
    port->write("1234", 4);   //发送“data”字符
}
