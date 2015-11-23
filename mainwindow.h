#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "serialcommunication.h"
#include <QTimer>
#include "registers.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void btnOpenSerialSlot();
    void bytesAvailable();
    void timeout();

private:
    Ui::MainWindow *ui;
    SerialCommunication serialComm;
    QTimer timer;
    bool serialOpen = false;
    Registers registers;
};

#endif // MAINWINDOW_H
