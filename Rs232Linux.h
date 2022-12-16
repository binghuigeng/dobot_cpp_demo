#ifndef Rs232LinuxH
#define Rs232LinuxH
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>
#include <qcoreapplication.h>
#include "Demo.h"

typedef unsigned char uint8;
typedef char int8;
typedef unsigned int uint16;
typedef signed int int16;

typedef void (*OnCommNotifyFunc)(void* arg);

const int package_length = 64;
const int buff_length = 64;
const int period_gap = 8*1000*1000;

class Rs232Linux {
   public:
    Rs232Linux(){};
    ~Rs232Linux(){}
    virtual void Init(unsigned char* strname);
    virtual void SetNotifyHandler(OnCommNotifyFunc notifyfunc, void* tag);
    virtual int WriteBuffer(char* buf, int len);
    virtual int ReadBuffer(char* buf, int len);
//    virtual int Open(const char* fname, int port, int baud, int parity);
    virtual int openport(char* Dev);
//    virtual int CloseCom();
    virtual int Open(int port,int baud);
    virtual int close();
    virtual unsigned char IsActive();
    virtual int Length();
    void uart_pthread(void* arg);

    void parse();

    int uartfd;
    pthread_t id1, idsend;
    unsigned char recbuff[buff_length];
    unsigned char rq_com_rcv_buff2[buff_length];

    int fd;
    int oflags;
    struct termios oldfopt;
    struct termios fopt;
    void OnComNotify(void* arg);
    OnCommNotifyFunc notfy;
    void* tempa;
    int setport(int fd, int baud, int databits, int stopbits, int parity);
//    int openport(const char* Dev);
    int readport(int fd, int maxwaittime);

    struct timespec time1 = {0, 0};
    struct timespec time2 = {0, 0};
    struct timespec pried = {0, 0};

    int rq_com_rcv_len2 = 0;
    Log& logger = Log::get_instance();
};
//---------------------------------------------------------------------------
#endif
