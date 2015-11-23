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
    void updateRegister(char address, QByteArray data);

    class Register
    {
        char address;
        QByteArray data;
        qint64 lastUpdated;
    };

    QHash<char, Register> registers;

signals:

public slots:

private:

};

#endif // REGISTERS_H
