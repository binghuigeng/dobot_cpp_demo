#include "SerialPort.h"
#include <QDebug>
#include "protocol.h"
#include "util.h"
#include <QCoreApplication>
#include "Demo.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <iostream>
using namespace std;

SerialPort::SerialPort(QObject *parent) : QObject(parent)
{
//    my_thread = new QThread();

    // 串口类必须在 moveToThread 之前初始化，否则程序会 crash（崩溃）
//    port = new QSerialPort();
}

SerialPort::~SerialPort()
{
    save_file.close();
//    port->close();
//    port->deleteLater();
//    my_thread->quit();
//    my_thread->wait();
//    my_thread->deleteLater();
}

void SerialPort::init_port()
{
//    qDebug() << "init_port_id is:" << QThread::currentThreadId();
//#ifdef WIN32
//    port->setPortName("COM7");                   //串口名 linux下写作/dev/ttyS1
//#else
//    port->setPortName("/dev/ttyUSB0");                   //串口名 windows下写作COM1
//#endif
//    port->setBaudRate(QSerialPort::Baud115200);                           //波特率
//    port->setBaudRate(460800);                         //波特率
//    port->setDataBits(QSerialPort::Data8);             //数据位
//    port->setStopBits(QSerialPort::OneStop);           //停止位
//    port->setParity(QSerialPort::NoParity);            //奇偶校验
//    port->setFlowControl(QSerialPort::NoFlowControl);  //流控制
//    if (port->open(QIODevice::ReadWrite))
//    {
//        qDebug() << "Port have been opened";
//    }
//    else
//    {
//        qDebug() << "open it failed";
//    }



//    auto app_path = QCoreApplication::applicationDirPath();
//    save_file.open(app_path.toStdString()+"/data.txt", std::ios::trunc);
//    start_t = std::chrono::steady_clock::now();
//    end_t = std::chrono::steady_clock::now();
//    gap = std::chrono::milliseconds(4);

    clock_gettime(CLOCK_MONOTONIC,&time1);
    clock_gettime(CLOCK_MONOTONIC,&time2);
//    connect(port, SIGNAL(readyRead()), this, SLOT(handle_data()), Qt::DirectConnection); //Qt::DirectConnection

//    int icount = 0;
//    while(port->waitForReadyRead(31)) {
//        end_t = std::chrono::steady_clock::now();
//        clock_gettime(CLOCK_MONOTONIC,&time2);
//        auto data = port->readAll();
//        if (data.isEmpty())
//            continue;

//        qDebug()  << ++icount << ","
//                  << time2.tv_nsec-time1.tv_nsec << "ns"
//                  << std::chrono::duration_cast<std::chrono::milliseconds>(end_t - start_t).count() << "ms"
//                     ;
//        start_t=end_t;
//        time1=time2;
//    }
struct timespec pried;
    char *portname = "/dev/ttyUSB0";
        int fd;
//        int wlen;

        fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC | O_NONBLOCK);
        if (fd < 0) {
            printf("Error opening %s: %s\n", portname, strerror(errno));
            return;
        }
        /*baudrate 115200, 8 bits, no parity, 1 stop bit */
        set_interface_attribs(fd, 460800);
        //set_mincount(fd, 0);                /* set to pure timed read */

        /* simple output */
//        wlen = write(fd, "Hello!\n", 7);
//        if (wlen != 7) {
//            printf("Error from write: %d, %d\n", wlen, errno);
//        }
//        tcdrain(fd);    /* delay for output */


        /* simple noncanonical input */
        static int icount = 0;
        uchar res = 0;
        static bool frame_head = false, frame_receive_start_flag = false;
        static int count = 0;
        static uchar frame_data[30] = {0};
        unsigned char buf[64];
        do {

            int rdlen;
            memset(&buf,'\0',sizeof(buf));
            clock_gettime(CLOCK_MONOTONIC,&time2);

            pried = time2;
            pried.tv_nsec += 4000000;
            while (pried.tv_nsec >= 1000000000) {
                pried.tv_nsec -= 1000000000;
                pried.tv_sec++;
            }
            rdlen = read(fd, buf, sizeof(buf));
            if (rdlen > 0) {

                std::cout << rdlen << "\n";
                for(int i=0; i< rdlen; i++)
                {
                    res = buf[i];
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
                            //判断是否为接收串口通讯协议格式定义的最后一个字节
                            if((count > 2) && (count == *((short*)&frame_data[1]) - 2 - 1))
                            {
                                frame_data[count] = res;
            //                    if(CRC16(&frame_data[1],frame_data[0]-1) != frame_data[frame_data[0]])
            //                    {
            //                        count = 0;
            //                        frame_receive_start_flag = false;
            //                        continue;
            //                    }
                                //判断命令号
                                switch (frame_data[3]) {
                                case 0x10:
                                {
                                    MSGO_FORCE_CONTROL *data = (MSGO_FORCE_CONTROL*)(char*)&frame_data[0];
                                    // qDebug("address: 0x%02X", data->address);
                                    // qDebug("length: 0x%04X", data->length);
                                    // qDebug("cmd: 0x%01X", data->cmd);
            //                         qDebug("Fx: %d", data->channel);
            //                         qDebug("Fy: %d", data->channel2);
            //                         qDebug("Fz: %d", data->channel3);
            //                         qDebug("Mx: %d", data->channel4);
            //                         qDebug("My: %d", data->channel5);
            //                         qDebug("Mz: %d", data->channel6);
                                    // qDebug("crc: 0x%04X", data->crc);
                                    // qDebug("icount: %d", ++icount);
                                    {
                                        std::cout << "1111111111111\n";
                                        save_file << std::left << ++icount << ","
    //                                              << time2.tv_nsec << "ms ,"
                                                  << data->channel / 1000.0 << "," << data->channel2 / 1000.0 << ","
                                               << data->channel3 / 1000.0 << "," << data->channel4 / 1000.0 << "," << data->channel5 / 1000.0 << ","
                                               << data->channel6 / 1000.0 << ","
            //                                   << Demo::getInstance()->getToolVectorActual(0) << "," << Demo::getInstance()->getToolVectorActual(1) << ","
            //                                   << Demo::getInstance()->getToolVectorActual(2)  << "," << Demo::getInstance()->getToolVectorActual(3) << ","
            //                                   << Demo::getInstance()->getToolVectorActual(4)  << "," << Demo::getInstance()->getToolVectorActual(5) << ","
            //                                   << Demo::getInstance()->getTimeStamp()
                                               << "\n";
                                    }
                                    break;
                                }
                                default:
                                    break;
                                }
                                count = 0;
                                frame_head = false;
                                frame_receive_start_flag = false;
                            }
                            else
                            {
                                frame_data[count] = res;
                                count++;
                                //判断数据帧长度是否为串口通讯协议格式定义的长度
                                if(count==3)
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
                            }
                        }
                        else
                        {
                            frame_head = false;
                            continue;
                        }
                    }
                }



            } else if (rdlen < 0) {
                //printf("Error from read: %d: %s\n", rdlen, strerror(errno));
            }
            /* repeat read to get full message */

clock_nanosleep(CLOCK_MONOTONIC,TIMER_ABSTIME,&pried,NULL);
//            std::cout<<   ++icount << ","
////                       << (time2.tv_sec-time1.tv_sec) << "s"<< " - "
//                      << (time2.tv_nsec-time1.tv_nsec)/1000/1000 << "ms"<<"\n";
//    //                  << std::chrono::duration_cast<std::chrono::milliseconds>(end_t - start_t).count() << "ms"
//                         ;


            time1=time2;
        } while (1);


}

void SerialPort::start()
{
    init_port();
//    this->moveToThread(my_thread);
//    port->moveToThread(my_thread);
    //    my_thread->start();  //启动线程
}

int SerialPort::set_interface_attribs(int fd, int speed)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        printf("Error from tcgetattr: %s\n", strerror(errno));
        return -1;
    }

    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;         /* 8-bit characters */
    tty.c_cflag &= ~PARENB;     /* no parity bit */
    tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;//    qDebug() << "init_port_id is:" << QThread::currentThreadId();
    //#ifdef WIN32
    //    port->setPortName("COM7");                   //串口名 linux下写作/dev/ttyS1
    //#else
    //    port->setPortName("/dev/ttyUSB0");                   //串口名 windows下写作COM1
    //#endif
    ////    port->setBaudRate(QSerialPort::Baud115200);                           //波特率
    //    port->setBaudRate(460800);                         //波特率
    //    port->setDataBits(QSerialPort::Data8);             //数据位
    //    port->setStopBits(QSerialPort::OneStop);           //停止位
    //    port->setParity(QSerialPort::NoParity);            //奇偶校验
    //    port->setFlowControl(QSerialPort::NoFlowControl);  //流控制
    //    if (port->open(QIODevice::ReadWrite))
    //    {
    //        qDebug() << "Port have been opened";
    //    }
    //    else
    //    {
    //        qDebug() << "open it failed";
    //    }

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("Error from tcsetattr: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

void SerialPort::handle_data()
{
    return;
#if 0
    QByteArray buf = port->readAll();
    qDebug() << QStringLiteral("data received(收到的数据):") << buf.toHex().data();
    qDebug() << "handing thread is:" << QThread::currentThreadId();
    emit receive_data(buf);
#endif

#if 1
//    qDebug() << "recv: ";
//    static int icount = 0;
//    uchar res;
//    static bool frame_head = false, frame_receive_start_flag = false;
//    static int count = 0;
//    static uchar frame_data[30] = {0};

    end_t = std::chrono::steady_clock::now();
//    if (std::chrono::duration_cast<std::chrono::milliseconds>(end_t - start_t) < gap )
//    {
////        start_t=end_t;
//        port->clear();
//        return;
//    }
    port->readAll();
//    qDebug("asfcvsdgvdfehnb");
//    qDebug() << std::left << ++icount << ","
//              << std::chrono::duration_cast<std::chrono::milliseconds>(end_t - start_t).count() << "ms \n";
    start_t=end_t;
    port->clear();
    return;

//    while(port->readAll()) {
//        save_file << std::left << ++icount << ","
//                  << std::chrono::duration_cast<std::chrono::milliseconds>(end_t - start_t).count() << "ms \n";
//        start_t=end_t;
//        port->clear();
//        return;
//    }

//    while (port->waitForReadyRead(3000)) {
////    std::cout<< port->
//    while(port->read((char*)&res,1)>0)
//    {
//        if(res==0xAA && !frame_head)
//        {
//            frame_head = true;
//            continue;
//        }

//        if (frame_head)
//        {
//            if(res==0x55 && !frame_receive_start_flag)
//            {
//                frame_receive_start_flag = true;
//                continue;
//            }

//            if(frame_receive_start_flag)
//            {
//                //判断是否为接收串口通讯协议格式定义的最后一个字节
//                if((count > 2) && (count == *((short*)&frame_data[1]) - 2 - 1))
//                {
//                    frame_data[count] = res;
////                    if(CRC16(&frame_data[1],frame_data[0]-1) != frame_data[frame_data[0]])
////                    {
////                        count = 0;
////                        frame_receive_start_flag = false;
////                        continue;
////                    }
//                    //判断命令号
//                    switch (frame_data[3]) {
//                    case 0x10:
//                    {
//                        MSGO_FORCE_CONTROL *data = (MSGO_FORCE_CONTROL*)(char*)&frame_data[0];
//                        // qDebug("address: 0x%02X", data->address);
//                        // qDebug("length: 0x%04X", data->length);
//                        // qDebug("cmd: 0x%01X", data->cmd);
////                         qDebug("Fx: %d", data->channel);
////                         qDebug("Fy: %d", data->channel2);
////                         qDebug("Fz: %d", data->channel3);
////                         qDebug("Mx: %d", data->channel4);
////                         qDebug("My: %d", data->channel5);
////                         qDebug("Mz: %d", data->channel6);
//                        // qDebug("crc: 0x%04X", data->crc);
//                        // qDebug("icount: %d", ++icount);
//                        {
//                            save_file << std::left << ++icount << ","
//                                      << std::chrono::duration_cast<std::chrono::milliseconds>(end_t - start_t).count() << "ms ,"
//                                      << data->channel / 1000.0 << "," << data->channel2 / 1000.0 << ","
//                                   << data->channel3 / 1000.0 << "," << data->channel4 / 1000.0 << "," << data->channel5 / 1000.0 << ","
//                                   << data->channel6 / 1000.0 << ","
////                                   << Demo::getInstance()->getToolVectorActual(0) << "," << Demo::getInstance()->getToolVectorActual(1) << ","
////                                   << Demo::getInstance()->getToolVectorActual(2)  << "," << Demo::getInstance()->getToolVectorActual(3) << ","
////                                   << Demo::getInstance()->getToolVectorActual(4)  << "," << Demo::getInstance()->getToolVectorActual(5) << ","
////                                   << Demo::getInstance()->getTimeStamp()
//                                   << "\n";
//                            start_t=end_t;
//                        }
//                        break;
//                    }
//                    default:
//                        break;
//                    }
//                    count = 0;
//                    frame_head = false;
//                    frame_receive_start_flag = false;
//                }
//                else
//                {
//                    frame_data[count] = res;
//                    count++;
//                    //判断数据帧长度是否为串口通讯协议格式定义的长度
//                    if(count==3)
//                    {
//                        switch (*((short*)&frame_data[1]))
//                        {
//                        case 0x0020:
//                        case 0x0026:
//                            break;
//                        default:
//                            count = 0;
//                            frame_head = false;
//                            frame_receive_start_flag = false;
//                            continue;
//                        }
//                    }
//                }
//            }
//            else
//            {
//                frame_head = false;
//                continue;
//            }
//        }
//    }
//    }
#endif
}

void SerialPort::write_data()
{
//    qDebug() << "write_id is:" << QThread::currentThreadId();
//    port->write("1234", 4);   //发送“data”字符
}
