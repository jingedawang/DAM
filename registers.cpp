#include "registers.h"
#include <QDateTime>

Registers::Registers(QObject *parent) : QObject(parent)
{

}

void Registers::updateRegister(char address, QByteArray data)
{
    registers[address].address = address;
    registers[address].data = data;
    registers[address].lastUpdated = QDateTime::currentMSecsSinceEpoch();
}

