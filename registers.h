#ifndef REGISTERS_H
#define REGISTERS_H

#include <QObject>
#include <QList>
#include <QHash>

class Registers : public QObject
{
    Q_OBJECT
public:
    explicit Registers(QObject *parent = 0);
    void updateRegister(uchar address, QByteArray data);

    class Register
    {
    public:
        uchar address;
        QByteArray data;
        qint64 lastUpdated;
    };

    QHash<uchar, Register> registers;

signals:

public slots:

private:

};

#endif // REGISTERS_H
