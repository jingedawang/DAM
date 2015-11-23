#ifndef SERIALCOMMUNICATION_H
#define SERIALCOMMUNICATION_H

#include <QObject>
#include <QList>
#include <QString>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

class SerialCommunication : public QObject
{
    Q_OBJECT
public:
    explicit SerialCommunication(QObject *parent = 0);
    ~SerialCommunication();
    QList<QString> getSerialPorts();
    bool openPort(int index);
    void send(const char *data, qint64 length);
    void send(const QByteArray data);
    void closePort();
    bool readBytes(QByteArray &bytes, int length);

signals:
    void bytesAvailable();

public slots:

private:
    QList<QSerialPortInfo> serials;
    QSerialPort selectedSerial;

    void refresh();
};

#endif // SERIALCOMMUNICATION_H
