#ifndef COMMAND_H
#define COMMAND_H

#include <QObject>

class Command : public QObject
{
    Q_OBJECT
public:
    explicit Command(QObject *parent = 0);
    explicit Command(bool, char, QByteArray);
    explicit Command(QByteArray bytes);
    QByteArray toByteArray();
    const char getAddress() const;
    const QByteArray &getData() const;

signals:

public slots:

private:
    QByteArray byteArray;
    bool isRead;
    char address;
    QByteArray data;
};

#endif // COMMAND_H
