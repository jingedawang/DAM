#include "command.h"
#include "QByteArray"
#include <QException>
#include <QDebug>

Command::Command(QObject *parent) : QObject(parent)
{

}

Command::Command(bool isRead, uchar address, QByteArray data)
{
    uchar read;
    if(isRead)
    {
        read = 0;
    }
    else
    {
        read = 3;
    }
    uchar byte0 = address | isRead << 6;
    QByteArray command;
    command.append(byte0);
    uchar byte1;
    uchar byte2;
    if(isRead)
    {
        byte1 = 0;
        byte2 = 0;
    }
    else
    {
        byte1 = data.right(2).at(0);
        byte2 = data.right(2).at(1);
    }
    command.append(byte1);
    command.append(byte2);
    uchar byte4 = byte0 + byte1 + byte2;
    command.append(byte4);
    if(command.length() != 4)
    {
//        throw QException();
        qDebug() << "构造命令时出错" << endl;
    }
    byteArray = command;
}

Command::Command(QByteArray bytes)
{
    byteArray = bytes;
    uchar byte0 = bytes.at(0);
    uchar byte1 = bytes.at(1);
    uchar byte2 = bytes.at(2);
    uchar byte3 = bytes.at(3);

    if(byte0 >> 6)
    {
        isRead = false;
    }
    else {
        isRead = true;
    }
    address = byte0 & 0x3F;
    data.append(byte1);
    data.append(byte2);

    if(byte3 != byte0 + byte1 + byte2)
    {
//        throw QException();
        qDebug() << "收到的回复数据错误" << endl;
        return;
    }
}

QByteArray Command::toByteArray()
{
    return byteArray;
}

const uchar Command::getAddress() const
{
    return address;
}

const QByteArray &Command::getData() const
{
    return data;
}

