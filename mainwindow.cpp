#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "command.h"
#include <QMessageBox>
#include <QtAlgorithms>
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
    connect(ui->btn_openSerial, SIGNAL(clicked(bool)), this, SLOT(btnOpenSerialSlot()));
    connect(&serialComm, SIGNAL(bytesAvailable()), this, SLOT(bytesAvailable()));
    for(int i=0; i<serialComm.getSerialPorts().count(); ++i)
    {
        ui->cboSerialNumber->addItem(serialComm.getSerialPorts().at(i));
    }

    connect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));
    //启动300ms定时器
//    timer.start(300);

    //单选按钮响应
    connect(ui->rdo_Q, SIGNAL(toggled(bool)), this, SLOT(rdoChecked(bool)));
    connect(ui->rdo_I, SIGNAL(toggled(bool)), this, SLOT(rdoChecked(bool)));

    //启动AD校准按钮
    connect(ui->btn_start_AD_calibrate, SIGNAL(clicked()), this, SLOT(btnStartADCalibrateClicked()));

    //测试用按钮
    connect(ui->btn_test_query, SIGNAL(clicked()), this, SLOT(btnTestQueryClicked()));

    //启动停止采样存储按钮
    connect(ui->btn_start_sample_storage, SIGNAL(clicked()), this, SLOT(btnStartSampleStorageClicked()));
    connect(ui->btn_stop_sample_storage, SIGNAL(clicked()), this, SLOT(btnStopSampleStorageClicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::btnOpenSerialSlot()
{
    if(serialOpen)
    {
        serialComm.closePort();
        serialOpen = false;
        ui->btn_openSerial->setText("打开串口");
    }
    else
    {
        //打开串口
        bool hasSuccess = serialComm.openPort(ui->cboSerialNumber->currentIndex());
        if(!hasSuccess)
        {
            QMessageBox msgBox;
            msgBox.setText("打开串口失败");
            msgBox.exec();
            return;
        }
        serialOpen = true;
        ui->btn_openSerial->setText("关闭串口");
    }
}

void MainWindow::bytesAvailable()
{
    QByteArray bytes;
    while(serialComm.readBytes(bytes, 4))
    {
        Command command(bytes);
        registers.updateRegister(command.getAddress(), command.getData());
    }

    //检查R0C寄存器
    if(registers.registers.contains(0x0c))
    {
        QByteArray &data = registers.registers[0x0c].data;
        //1:0位
        //系统状态
        if(!(data.at(1) & 0x03))
        {
            ui->edt_system_state->setText("空闲");
        }
        else if((data.at(1) & 0x02) && !(data.at(1) & 0x01))
        {
            ui->edt_system_state->setText("采样存储功能");
        }
        else if((data.at(1) & 0x02) && (data.at(1) & 0x01))
        {
            ui->edt_system_state->setText("数据导出功能");
        }

        //第3位
        //采样存储执行状态
        if(data.at(1) & 0x08)
        {
            ui->edt_sample_store_execute_state->setText("完成");
        }
        else
        {
            ui->edt_sample_store_execute_state->setText("正在执行");
        }

        //第4位
        //数据导出执行状态
        if(data.at(1) & 0x10)
        {
            ui->edt_data_export_excute_state->setText("完成");
        }
        else
        {
            ui->edt_data_export_excute_state->setText("正在执行");
        }

        //第5位
        //AD配置状态
        if(data.at(1) & 0x20)
        {
            ui->edt_AD_config_state->setText("完成配置");
        }
        else
        {
            ui->edt_AD_config_state->setText("正在配置或未配置");
        }

        //第6位
        //AD校准状态
        if(data.at(1) & 0x40)
        {
            ui->edt_AD_calibrate_state->setText("校准完成");
        }
        else
        {
            ui->edt_AD_calibrate_state->setText("正在校准或未校准");
        }

        //第7位
        //时钟锁定状态
        if(data.at(1) & 0x80)
        {
            ui->edt_clock_lock_state->setText("锁定");
        }
        else
        {
            ui->edt_clock_lock_state->setText("失锁");
        }

        //第8位
        //AD校准运行状态
        if(data.at(0) & 0x01)
        {
            ui->edt_AD_calibrate_running_state->setText("正在运行");
        }
        else
        {
            ui->edt_AD_calibrate_running_state->setText("未运行");
        }

        //第9位
        //I路数据有效指示
        if(data.at(0) & 0x02)
        {
            ui->edt_I_data_valid->setText("数据有效");
        }
        else
        {
            ui->edt_I_data_valid->setText("数据无效");
        }

        //第A位
        //Q路数据有效指示
        if(data.at(0) & 0x04)
        {
            ui->edt_Q_data_valid->setText("数据有效");
        }
        else
        {
            ui->edt_Q_data_valid->setText("数据无效");
        }
    }

    //检查R1B,R1A,R19寄存器
    //当前帧号
    if(registers.registers.contains(0x1b) && registers.registers.contains(0x1a) && registers.registers.contains(0x19))
    {
        //当三个寄存器值都到达才执行
        if(qAbs(registers.registers[0x1b].lastUpdated - registers.registers[0x1a].lastUpdated) < 300
                && qAbs(registers.registers[0x1a].lastUpdated - registers.registers[0x19].lastUpdated) < 300
                && qAbs(registers.registers[0x19].lastUpdated - registers.registers[0x1b].lastUpdated) < 300)
        {
            QByteArray frameNum;
            frameNum.append(registers.registers[0x1b].data);
            frameNum.append(registers.registers[0x1a].data);
            frameNum.append(registers.registers[0x19].data);
            ui->edt_current_frame->setText(QString::number(frameNum.toInt()));
        }
    }

    //检查R11,R10寄存器
    //I路触发计数
    if(registers.registers.contains(0x11) && registers.registers.contains(0x10))
    {
        //当两个寄存器值都到达才执行
        if(qAbs(registers.registers[0x11].lastUpdated - registers.registers[0x10].lastUpdated) < 300)
        {
            QByteArray ITriggerCount;
            ITriggerCount.append(registers.registers[0x11].data);
            ITriggerCount.append(registers.registers[0x10].data);
            ui->edt_I_trigger_count->setText(QString::number(ITriggerCount.toInt()));
        }
    }

    //检查R14寄存器
    //I路采样幅度MAX
    if(registers.registers.contains(0x14))
    {
        ui->edt_I_sample_amplitude_max->setText(QString::number(registers.registers[0x14].data.toInt()));
    }

    //检查R15寄存器
    //I路采样幅度MIN
    if(registers.registers.contains(0x15))
    {
        ui->edt_I_sample_amplitude_min->setText(QString::number(registers.registers[0x15].data.toInt()));
    }

    //检查R18寄存器
    if(registers.registers.contains(0x18))
    {
        //第0位
        //I路超幅指示
        if(registers.registers[0x18].data.at(1) & 0x01)
        {
            ui->edt_I_sample_amplitude_max->setText("超幅度");
        }
        else
        {
            ui->edt_I_sample_amplitude_max->setText("正常");
        }
    }

    //检查R13,R12寄存器
    //Q路触发计数
    if(registers.registers.contains(0x13) && registers.registers.contains(0x12))
    {
        //当两个寄存器值都到达才执行
        if(qAbs(registers.registers[0x13].lastUpdated - registers.registers[0x12].lastUpdated) < 300)
        {
            QByteArray QTriggerCount;
            QTriggerCount.append(registers.registers[0x13].data);
            QTriggerCount.append(registers.registers[0x12].data);
            ui->edt_Q_trigger_count->setText(QString::number(QTriggerCount.toInt()));
        }
    }

    //检查R16寄存器
    //Q路采样幅度MAX
    if(registers.registers.contains(0x16))
    {
        ui->edt_Q_sample_amplitude_max->setText(QString::number(registers.registers[0x16].data.toInt()));
    }

    //检查R17寄存器
    //Q路采样幅度MIN
    if(registers.registers.contains(0x17))
    {
        ui->edt_Q_sample_amplitude_min->setText(QString::number(registers.registers[0x17].data.toInt()));
    }

    //检查R18寄存器
    if(registers.registers.contains(0x18))
    {
        //第1位
        //Q路超幅指示
        if(registers.registers[0x18].data.at(1) & 0x02)
        {
            ui->edt_I_sample_amplitude_max->setText("超幅度");
        }
        else
        {
            ui->edt_I_sample_amplitude_max->setText("正常");
        }
    }
}

void MainWindow::timeout()
{
    if(!serialOpen)
    {
        return;
    }
    Command command1(true, 0x0c, 0);
    Command command2(true, 0x1b, 0);
    Command command3(true, 0x1a, 0);
    Command command4(true, 0x19, 0);
    Command command5(true, 0x11, 0);
    Command command6(true, 0x10, 0);
    Command command7(true, 0x14, 0);
    Command command8(true, 0x15, 0);
    Command command9(true, 0x18, 0);
    Command command10(true, 0x13, 0);
    Command command11(true, 0x12, 0);
    Command command12(true, 0x16, 0);
    Command command13(true, 0x17, 0);
    Command command14(true, 0x18, 0);
    serialComm.send(command1.toByteArray());
    serialComm.send(command2.toByteArray());
    serialComm.send(command3.toByteArray());
    serialComm.send(command4.toByteArray());
    serialComm.send(command5.toByteArray());
    serialComm.send(command6.toByteArray());
    serialComm.send(command7.toByteArray());
    serialComm.send(command8.toByteArray());
    serialComm.send(command9.toByteArray());
    serialComm.send(command10.toByteArray());
    serialComm.send(command11.toByteArray());
    serialComm.send(command12.toByteArray());
    serialComm.send(command13.toByteArray());
    serialComm.send(command14.toByteArray());

}

void MainWindow::rdoChecked(bool checked)
{
    if(!checked)
    {
        return;
    }
    uchar byte0 = 0;
    uchar byte1;
    if(ui->rdo_Q->isChecked())
    {
        byte1 = 0x06;
    }
    else if(ui->rdo_I->isChecked())
    {
        byte1 = 0x0a;
    }
    QByteArray data;
    data.append(byte0).append(byte1);
    Command command(false, 0x0d, data);
    serialComm.send(command.toByteArray());
    QMessageBox msgBox;
    msgBox.setText("请启动AD校准功能");
    msgBox.exec();
}

void MainWindow::btnStartADCalibrateClicked()
{
    QByteArray data = registers.registers[0x0c].data;
    if(!(data.at(0) & 0x01) && (data.at(1) & 0x80) && (data.at(1) & 0x20))
    {
        QMessageBox msgBox;
        msgBox.setText("将启动AD校准功能");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();
        if(ret == QMessageBox::Yes)
        {
            uchar byte0 = 0;
            uchar byte1_first;
            uchar byte1_second;
            if(ui->rdo_Q->isChecked())
            {
                byte1_first = 0x04;
                byte1_second = 0x06;
            }
            else if(ui->rdo_I->isChecked())
            {
                byte1_first = 0x08;
                byte1_second = 0x0a;
            }
            QByteArray data_first, data_second;
            data_first.append(byte0).append(byte1_first);
            data_second.append(byte0).append(byte1_second);
            Command command_first(false, 0x0d, data_first);
            Command command_second(false, 0x0d, data_second);
            serialComm.send(command_first.toByteArray());
            serialComm.send(command_second.toByteArray());
        }
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("当前设备不能执行AD校准操作");
        msgBox.exec();
    }
}

void MainWindow::btnTestQueryClicked()
{
    timeout();
}

void MainWindow::btnStartSampleStorageClicked()
{
    QByteArray data = registers.registers[0x0c].data;
    if(data.at(1) & 0x40 && (data.at(0) & 0x02 || data.at(0) & 0x04) && !(data.at(1) & 0x02) && !(data.at(1) & 0x01))
    {
        QMessageBox msgBox;
        msgBox.setText("将启动采样存储功能");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Yes);
        msgBox.setButtonText(QMessageBox::Yes, "确认");
        msgBox.setButtonText(QMessageBox::Cancel, "取消");
        int ret = msgBox.exec();
        if(ret == QMessageBox::Yes)
        {
            uint triggerCount = ui->edt_trigger_count->text().toInt();
            uint sampleNum = ui->edt_sample_num->text().toInt();
            uint preSampleNum = ui->edt_pre_sample_num->text().toInt();
            uint startFrameno = ui->edt_frame_startno->text().toInt();
            uint storageFrameNum = ui->edt_storage_frame_num->text().toInt();
            qDebug() << "hhahfiehgeowghewio" << endl;
        }
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("当前设备不能启动采样存储功能");
        msgBox.exec();
    }

}

void MainWindow::btnStopSampleStorageClicked()
{

}
