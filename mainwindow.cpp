#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /*
     * 以下为测试代码，点击test按钮发消息"I love you."
     * bytesAvailable槽接收serialComm的bytesAvailable信号
     * 将接收到的数据打印在控制台
     */
    connect(ui->btn_test, SIGNAL(clicked(bool)), this, SLOT(btnTestSlot()));
    connect(&serialComm, SIGNAL(bytesAvailable(QByteArray)), this, SLOT(bytesAvailable(QByteArray)));
    for(int i=0; i<serialComm.getSerialPorts().count(); ++i)
    {
        ui->cboSerialNumber->addItem(serialComm.getSerialPorts().at(i));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::btnTestSlot()
{
    //打开串口
    serialComm.openPort(ui->cboSerialNumber->currentIndex());
    //发送数据
    serialComm.send("I love you.");
}

void MainWindow::bytesAvailable(const QByteArray data)
{
    qDebug() << data << endl;
}
