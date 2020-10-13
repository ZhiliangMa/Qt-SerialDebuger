#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // 新建一串口对象
    mySerialPort = new QSerialPort(this);
    //QSerialPort(mySerialPort);

    // 串口接收，信号槽关联
    connect(mySerialPort, SIGNAL(readyRead()), this, SLOT(serialPortRead_Slot()));
}

Widget::~Widget()
{
    delete ui;
}

// 串口接收显示，槽函数
void Widget::serialPortRead_Slot()
{
    /*QString recBuf;
    recBuf = QString(mySerialPort->readAll());*/

    QByteArray recBuf;
    recBuf = mySerialPort->readAll();

    // 判断是否为16进制接收，将以后接收的数据全部转换为16进制显示（先前接收的部分在多选框槽函数中进行转换。最好多选框和接收区组成一个自定义控件，方便以后调用）
    if(ui->chkRec->checkState() == false)
    {
        // 在当前位置插入文本，不会发生换行。如果没有移动光标到文件结尾，会导致文件超出当前界面显示范围，界面也不会向下滚动。
        ui->txtRec->insertPlainText(recBuf);
    }else{
        // 16进制显示，并转换为大写
        QString str1 = recBuf.toHex().toUpper();//.data();
        // 添加空格
        QString str2;
        for(int i = 0; i<str1.length (); i+=2)
        {
            str2 += str1.mid (i,2);
            str2 += " ";
        }
        ui->txtRec->insertPlainText(str2);
        //ui->txtRec->insertPlainText(recBuf.toHex());
    }

    // 移动光标到文本结尾
    ui->txtRec->moveCursor(QTextCursor::End);

    // 将文本追加到末尾显示，会导致插入的文本换行
    /*ui->txtRec->append(recBuf);*/

    /*// 在当前位置插入文本，不会发生换行。如果没有移动光标到文件结尾，会导致文件超出当前界面显示范围，界面也不会向下滚动。
    ui->txtRec->insertPlainText(recBuf);
    ui->txtRec->moveCursor(QTextCursor::End);*/

    // 利用一个QString去获取消息框文本，再将新接收到的消息添加到QString尾部，但感觉效率会比当前位置插入低。也不会发生换行
    /*QString txtBuf;
    txtBuf = ui->txtRec->toPlainText();
    txtBuf += recBuf;
    ui->txtRec->setPlainText(txtBuf);
    ui->txtRec->moveCursor(QTextCursor::End);*/

    // 利用一个QString去缓存接收到的所有消息，效率会比上面高一点。但清空接收的时候，要将QString一并清空。
    /*static QString txtBuf;
    txtBuf += recBuf;
    ui->txtRec->setPlainText(txtBuf);
    ui->txtRec->moveCursor(QTextCursor::End);*/
}

// 打开/关闭串口 槽函数
void Widget::on_btnSwitch_clicked()
{

    QSerialPort::BaudRate baudRate;
    QSerialPort::DataBits dataBits;
    QSerialPort::StopBits stopBits;
    QSerialPort::Parity   checkBits;

    // 获取串口波特率
    // 有没有直接字符串转换为 int的方法？？？
    //baudRate = ui->cmbBaudRate->currentText().toInt();
    if(ui->cmbBaudRate->currentText() == "9600"){
        baudRate = QSerialPort::Baud9600;
    }else if(ui->cmbBaudRate->currentText() == "38400"){
        baudRate = QSerialPort::Baud38400;
    }else if(ui->cmbBaudRate->currentText() == "115200"){
        baudRate = QSerialPort::Baud115200;
    }else{

    }

    // 获取串口数据位
    if(ui->cmbData->currentText() == "5"){
        dataBits = QSerialPort::Data5;
    }else if(ui->cmbData->currentText() == "6"){
        dataBits = QSerialPort::Data6;
    }else if(ui->cmbData->currentText() == "7"){
        dataBits = QSerialPort::Data7;
    }else if(ui->cmbData->currentText() == "8"){
        dataBits = QSerialPort::Data8;
    }else{

    }

    // 获取串口停止位
    if(ui->cmbStop->currentText() == "1"){
        stopBits = QSerialPort::OneStop;
    }else if(ui->cmbStop->currentText() == "1.5"){
        stopBits = QSerialPort::OneAndHalfStop;
    }else if(ui->cmbStop->currentText() == "2"){
        stopBits = QSerialPort::TwoStop;
    }else{

    }

    // 获取串口奇偶校验位
    if(ui->cmbCheck->currentText() == "无"){
        checkBits = QSerialPort::NoParity;
    }else if(ui->cmbCheck->currentText() == "奇校验"){
        checkBits = QSerialPort::OddParity;
    }else if(ui->cmbCheck->currentText() == "偶校验"){
        checkBits = QSerialPort::EvenParity;
    }else{

    }

    // 想想用 substr strchr怎么从带有信息的字符串中提前串口号字符串
    // 初始化串口属性，设置 端口号、波特率、数据位、停止位、奇偶校验位数
    mySerialPort->setBaudRate(baudRate);
    mySerialPort->setDataBits(dataBits);
    mySerialPort->setStopBits(stopBits);
    mySerialPort->setParity(checkBits);
    //mySerialPort->setPortName(ui->cmbSerialPort->currentText());// 不匹配带有串口设备信息的文本
    // 匹配带有串口设备信息的文本
    QString spTxt = ui->cmbSerialPort->currentText();
    spTxt = spTxt.section(':', 0, 0);//spTxt.mid(0, spTxt.indexOf(":"));
    //qDebug() << spTxt;
    mySerialPort->setPortName(spTxt);

    // 根据初始化好的串口属性，打开串口
    // 如果打开成功，反转打开按钮显示和功能。打开失败，无变化，并且弹出错误对话框。
    if(ui->btnSwitch->text() == "打开串口"){
        if(mySerialPort->open(QIODevice::ReadWrite) == true){
            //QMessageBox::
            ui->btnSwitch->setText("关闭串口");
            // 让端口号下拉框不可选，避免误操作（选择功能不可用，控件背景为灰色）
            ui->cmbSerialPort->setEnabled(false);
        }else{
            QMessageBox::critical(this, "错误提示", "串口打开失败！！！\r\n该串口可能被占用\r\n请选择正确的串口");
        }
    }else{
        mySerialPort->close();
        ui->btnSwitch->setText("打开串口");
        // 端口号下拉框恢复可选，避免误操作
        ui->cmbSerialPort->setEnabled(true);
    }

}

// 发送按键槽函数
// 如果勾选16进制发送，按照asc2的16进制发送
void Widget::on_btnSend_clicked()
{
    // 判断是否为16进制发送，将发送区全部的asc2转换为16进制字符串显示，发送的时候转换为16进制发送
    if(ui->chkSend->checkState() == false){
        // 字符串形式发送
        mySerialPort->write(ui->txtSend->toPlainText().toLocal8Bit().data());
    }else{
        // 16进制发送
        mySerialPort->write(QByteArray::fromHex(ui->txtSend->toPlainText().toUtf8()).data());
    }

}

void Widget::on_btnClearRec_clicked()
{
    ui->txtRec->clear();
}

void Widget::on_btnClearSend_clicked()
{
    ui->txtSend->clear();
}

// 先前接收的部分在多选框状态转换槽函数中进行转换。（最好多选框和接收区组成一个自定义控件，方便以后调用）
void Widget::on_chkRec_stateChanged(int arg1)
{
    // 获取多选框状态，未选为0，选中为2
    int status = arg1;
    // 获取文本字符串
    QString txtBuf = ui->txtRec->toPlainText();

    // 为0时，多选框未被勾选，接收区先前接收的16进制数据转换为asc2字符串格式
    if(status == 0){

        QByteArray str1 = QByteArray::fromHex(txtBuf.toUtf8());
        // 文本控件清屏，显示新文本
        ui->txtRec->clear();
        ui->txtRec->insertPlainText(str1);
        // 移动光标到文本结尾
        ui->txtRec->moveCursor(QTextCursor::End);

    }else{// 不为0时，多选框被勾选，接收区先前接收asc2字符串转换为16进制显示

        QByteArray str1 = txtBuf.toUtf8().toHex().toUpper();
        // 添加空格
        QByteArray str2;
        for(int i = 0; i<str1.length (); i+=2)
        {
            str2 += str1.mid (i,2);
            str2 += " ";
        }
        // 文本控件清屏，显示新文本
        ui->txtRec->clear();
        ui->txtRec->insertPlainText(str2);
        // 移动光标到文本结尾
        ui->txtRec->moveCursor(QTextCursor::End);

    }
}

// 先前发送区的部分在多选框状态转换槽函数中进行转换。（最好多选框和发送区组成一个自定义控件，方便以后调用）
void Widget::on_chkSend_stateChanged(int arg1)
{
    // 获取多选框状态，未选为0，选中为2
    int status = arg1;
    // 获取文本字符串
    QString txtBuf = ui->txtSend->toPlainText();

    // 为0时，多选框未被勾选，将先前的发送区的16进制字符串转换为asc2字符串
    if(status == 0){

        QByteArray str1 = QByteArray::fromHex(txtBuf.toUtf8());
        // 文本控件清屏，显示新文本
        ui->txtSend->clear();
        ui->txtSend->insertPlainText(str1);
        // 移动光标到文本结尾
        ui->txtSend->moveCursor(QTextCursor::End);

    }else{// 多选框被勾选，将先前的发送区的asc2字符串转换为16进制字符串

        QByteArray str1 = txtBuf.toUtf8().toHex().toUpper();
        // 添加空格
        QByteArray str2;
        for(int i = 0; i<str1.length (); i+=2)
        {
            str2 += str1.mid (i,2);
            str2 += " ";
        }
        // 文本控件清屏，显示新文本
        ui->txtSend->clear();
        ui->txtSend->insertPlainText(str2);
        // 移动光标到文本结尾
        ui->txtSend->moveCursor(QTextCursor::End);

    }
}
