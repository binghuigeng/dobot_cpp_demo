#include <pthread.h>

#include <QCoreApplication>
#include <iostream>

#include "Demo.h"
#include "Rs232Linux.h"
#include "SerialPort.h"

// #include <errno.h>
// #include <fcntl.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <termios.h>
// #include <unistd.h>

// #include <iostream>
// using namespace std;

// int set_interface_attribs(int fd, int speed)
//{
//     struct termios tty;

//    if (tcgetattr(fd, &tty) < 0) {
//        printf("Error from tcgetattr: %s\n", strerror(errno));
//        return -1;
//    }

//    cfsetospeed(&tty, (speed_t)speed);
//    cfsetispeed(&tty, (speed_t)speed);

//    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
//    tty.c_cflag &= ~CSIZE;
//    tty.c_cflag |= CS8;         /* 8-bit characters */
//    tty.c_cflag &= ~PARENB;     /* no parity bit */
//    tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
//    tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

//    /* setup for non-canonical mode */
//    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
//    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
//    tty.c_oflag &= ~OPOST;

//    /* fetch bytes as they become available */
//    tty.c_cc[VMIN] = 1;
//    tty.c_cc[VTIME] = 1;

//    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
//        printf("Error from tcsetattr: %s\n", strerror(errno));
//        return -1;
//    }
//    return 0;
//}

// void set_mincount(int fd, int mcount)
//{
//     struct termios tty;

//    if (tcgetattr(fd, &tty) < 0) {
//        printf("Error tcgetattr: %s\n", strerror(errno));
//        return;
//    }

//    tty.c_cc[VMIN] = mcount ? 1 : 0;
//    tty.c_cc[VTIME] = 5;        /* half second timer */

//    if (tcsetattr(fd, TCSANOW, &tty) < 0)
//        printf("Error tcsetattr: %s\n", strerror(errno));
//}

std::atomic<bool> Demo::isrun = {true};
Rs232Linux test;
void* serial_start(void*) {
    test.Open(1, 1);
    return nullptr;
}

void* log_start(void*) {
    Log::get_instance().start();
    return nullptr;
}

int main(int argc, char* argv[]) {
    QCoreApplication a(argc, argv);

    // 启动Demo线程
    //    Demo::getInstance()->start();

    //     启动串口模块
    int ret;
    pthread_t uartThread,logThread;
    //mlockall(MCL_CURRENT | MCL_FUTURE);//锁定内存
    struct sched_param param = { .sched_priority = sched_get_priority_max(SCHED_FIFO) };
    pthread_attr_t thattr;

    // serial port
    pthread_attr_init(&thattr);
    pthread_attr_setdetachstate(&thattr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setinheritsched(&thattr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&thattr, SCHED_FIFO);
    pthread_attr_setschedparam(&thattr,&param);
    ret = pthread_create(&uartThread, &thattr, &serial_start, NULL);
    if(ret){
        std::cout <<"ERROR : pthread_create uartThread task failed\n";
    }else {
        pthread_detach(uartThread);
    }

    // log
    param.sched_priority = 98;
    pthread_attr_setschedpolicy(&thattr, SCHED_FIFO);
    pthread_attr_setschedparam(&thattr, &param);
    ret = pthread_create(&logThread, &thattr, &log_start, NULL);
    if (ret) {
        std::cout << "ERROR : pthread_create logThread task failed\n";
    } else {
        pthread_detach(logThread);
    }

    ret = a.exec();
    {
        //        Demo::deleteInstance();
    }

    Demo::isrun = false;
    return ret;
}
