#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "serialcommunication.h"

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
    void btnTestSlot();
    void bytesAvailable(const QByteArray);

private:
    Ui::MainWindow *ui;
    SerialCommunication serialComm;
};

#endif // MAINWINDOW_H
