#ifndef HANDLE_H
#define HANDLE_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QTimer>
#include "SerialPort.h"

class Handle : public QObject
{
    Q_OBJECT
public:
    ~Handle();

    //单例模式
    static Handle *getInstance();
    static void deleteInstance();
    void initialize();

signals:

public slots:
    void on_receive(QByteArray tmpdata);

private:
    explicit Handle(QObject *parent = nullptr);
    static Handle *m_instance;
    static QMutex staticMutex;
    //1秒定时器
    QTimer timer1s;

    SerialPort *local_serial;
};

#endif // HANDLE_H
