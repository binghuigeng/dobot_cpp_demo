#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QObject>

#pragma pack(1)

//串口通讯协议格式
struct MSGO_FORCE_CONTROL{
    unsigned char address;    //地址
    short length;             //数据帧长度
    unsigned char cmd;        //命令号
    int channel;              //通道1数据
    int channel2;              //通道2数据
    int channel3;              //通道3数据
    int channel4;              //通道4数据
    int channel5;              //通道5数据
    int channel6;              //通道6数据
    unsigned short crc;       //校验
};

#pragma pack()

#endif // PROTOCOL_H
