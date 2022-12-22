#include <pthread.h>

#include <QCoreApplication>
#include <iostream>

#include "Demo.h"
#include "Rs232Linux.h"

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
    Demo::getInstance()->start();

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
        Demo::deleteInstance();
    }

    Demo::isrun = false;
    return ret;
}
