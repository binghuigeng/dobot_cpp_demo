#include "SerialPort.h"
#include <QDebug>
#include "protocol.h"
#include "util.h"

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
#if 0
    QByteArray buf = port->readAll();
    qDebug() << QStringLiteral("data received(收到的数据):") << buf.toHex().data();
    qDebug() << "handing thread is:" << QThread::currentThreadId();
    emit receive_data(buf);
#endif

#if 1
    qDebug() << "recv: ";
    static int icount = 0;
    uchar res;
    static bool frame_head = false, frame_receive_start_flag = false;
    static int count = 0;
    static uchar frame_data[30] = {0};

    while(port->read((char*)&res,1)>0)
    {
        if(res==0xAA && !frame_head)
        {
            frame_head = true;
            continue;
        }

        if (frame_head)
        {
            if(res==0x55 && !frame_receive_start_flag)
            {
                frame_receive_start_flag = true;
                continue;
            }

            if(frame_receive_start_flag)
            {
                if((count > 2) && (count == *((short*)&frame_data[1]) - 2))
                {
//                    if(CRC16(&frame_data[1],frame_data[0]-1) != frame_data[frame_data[0]])
//                    {
//                        count = 0;
//                        frame_receive_start_flag = false;
//                        continue;
//                    }
                    MSGO_FORCE_CONTROL *data = (MSGO_FORCE_CONTROL*)(char*)&frame_data[0];
                    qDebug("address: 0x%02X", data->address);
                    qDebug("length: 0x%04X", data->length);
                    qDebug("cmd: 0x%01X", data->cmd);
                    qDebug("channel: 0x%08X", data->channel);
                    qDebug("channel2: 0x%08X", data->channel2);
                    qDebug("channel3: 0x%08X", data->channel3);
                    qDebug("channel4: 0x%08X", data->channel4);
                    qDebug("channel5: 0x%08X", data->channel5);
                    qDebug("channel6: 0x%08X", data->channel6);
                    qDebug("crc: 0x%04X", data->crc);
                    qDebug("icount: %d", ++icount);

                    count = 0;
                    frame_head = false;
                    frame_receive_start_flag = false;
                }
                else
                {
                    if(count==2)
                    {
                        switch (*((short*)&frame_data[1]))
                        {
                        case 0x0020:
                        case 0x0026:
                            break;
                        default:
                            count = 0;
                            frame_head = false;
                            frame_receive_start_flag = false;
                            continue;
                        }
                    }
                    frame_data[count] = res;
                    count++;
                }
            }
            else
            {
                frame_head = false;
                continue;
            }
        }
    }
#endif
}

void SerialPort::write_data()
{
    qDebug() << "write_id is:" << QThread::currentThreadId();
    port->write("1234", 4);   //发送“data”字符
}
