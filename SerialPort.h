#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QSerialPort>
#include <QThread>
#include <fstream>
#include <iomanip>
#include <chrono>

class SerialPort : public QObject
{
    Q_OBJECT
public:
    explicit SerialPort(QObject *parent = nullptr);
    ~SerialPort();

    void init_port();  //初始化串口
    void start();  //启动串口


    int set_interface_attribs(int fd, int speed);

signals:
    //接收数据
    void receive_data(QByteArray tmp);

public slots:
    void handle_data();  //处理接收到的数据
    void write_data();     //发送数据

private:
    QThread *my_thread;
    QSerialPort *port;
    std::ofstream save_file;
    std::chrono::steady_clock::time_point start_t,end_t;
    std::chrono::milliseconds gap;
};

#endif // SERIALPORT_H
