#include "Handle.h"
#include <QDebug>

Handle::Handle(QObject *parent) : QObject(parent)
{
    local_serial = new SerialPort();
    connect(&timer1s, SIGNAL(timeout()), local_serial, SLOT(write_data()), Qt::QueuedConnection);
    connect(local_serial, SIGNAL(receive_data(QByteArray)), this, SLOT(on_receive(QByteArray)), Qt::QueuedConnection);
}

Handle::~Handle()
{
    local_serial->deleteLater();
}

Handle *Handle::getInstance()
{
    if(!m_instance)
    {
        QMutexLocker locker(&staticMutex);
        if(nullptr == m_instance)
        {
            m_instance = new Handle();
        }
    }
    return m_instance;
}

void Handle::deleteInstance()
{
    QMutexLocker locker(&staticMutex);
    if(m_instance)
    {
        delete m_instance;
        m_instance = nullptr;
    }
}

void Handle::initialize()
{
    timer1s.setInterval(1000);
//    timer1s.start();
}

void Handle::on_receive(QByteArray tmpdata)
{
    if (!tmpdata.isEmpty())
    {
//        int data_len = tmpdata.length();
//        //handle the Hex Case;
//        for (int var = 0; var < data_len; var++)
//        {
//            QString tempStr;
//            tempStr = QString::number(tmpdata.at(var), 16);
//            if (tmpdata.at(var) < 0x10)
//            {
//                tempStr = "0" + tempStr;
//            }
//            tempStr = /*"0x" + */tempStr.toUpper() + " ";

//            Str += tempStr;
//        }
        //handle the Hex Case;
        QString hex_data = tmpdata.toHex().data(); // 将buf里的数据转换为16进制
        hex_data = hex_data.toUpper(); // 转换为大写
        QString hex_str;
        // 添加空格
        for (int i=0; i< hex_data.length(); i+=2)
        {
            QString st = hex_data.mid(i,2);
            hex_str += st;
            hex_str += ' ';
        }
        qDebug() << "recv: " << hex_str;
    }
}

//静态成员变量初始化
Handle *Handle::m_instance = nullptr;
QMutex Handle::staticMutex;
