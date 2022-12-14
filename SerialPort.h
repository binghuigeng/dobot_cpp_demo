#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QSerialPort>
#include <QThread>
#include <fstream>
#include <iomanip>

class SerialPort : public QObject
{
    Q_OBJECT
public:
    explicit SerialPort(QObject *parent = nullptr);
    ~SerialPort();

    void init_port();  //初始化串口
    void start();  //启动串口

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
};

#endif // SERIALPORT_H
