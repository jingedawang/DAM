#include "serialcommunication.h"
#include <QDebug>

SerialCommunication::SerialCommunication(QObject *parent) : QObject(parent)
{
    connect(&selectedSerial, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

SerialCommunication::~SerialCommunication()
{
    selectedSerial.close();
}

QList<QString> SerialCommunication::getSerialPorts()
{
    /*
     * 返回串口列表
     */
    refresh();
    QList<QString> list;
    for(int i=0; i<serials.count(); i++)
    {
        list.append(serials.at(i).portName());
    }
    return list;
}

bool SerialCommunication::openPort(int index)
{
    if(selectedSerial.portName() != serials.at(index).portName() && selectedSerial.isOpen())
    {
        //如果原来的端口没关，则关闭
        selectedSerial.close();
    }
    if(selectedSerial.portName() == serials.at(index).portName() && selectedSerial.isOpen())
    {
        //端口不变且已经打开，直接退出
        return true;
    }
    //选中index指向的端口
    selectedSerial.setPort(serials.at(index));
    //打开新端口
    bool success = selectedSerial.open(QSerialPort::ReadWrite);
    if(!success)
    {
        qDebug() << "打开端口" << index << "失败" << endl;
        return false;
    }
    return true;
}

void SerialCommunication::send(const char *data, qint64 length)
{
    selectedSerial.write(data, length);
}

void SerialCommunication::send(const QByteArray data)
{
    selectedSerial.write(data);
}

void SerialCommunication::readyRead()
{
    const QByteArray bytes = selectedSerial.readAll();
    emit bytesAvailable(bytes);
}

void SerialCommunication::refresh()
{
    serials = QSerialPortInfo::availablePorts();
}
