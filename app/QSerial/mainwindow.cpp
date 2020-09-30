#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mainWindowInit();                    //主窗口初始化
    labelInit();                         //标签初始化
    textPlainInit();                     //textPlain初始化
    comboBoxInit();                      //初始化comboBox
    portInit();                          //初始化串口
    btnInit();                           //按钮初始化
}

MainWindow::~MainWindow()
{
    delete ui;
}

/* 初始化主窗口 */
void MainWindow::mainWindowInit(void)
{
    //设置窗口位置
//    this->setGeometry(0, 0, QApplication::desktop()->screenGeometry().width(),
//                      QApplication::desktop()->screenGeometry().height());
    this->setGeometry(0, 0, SCREEN_X_SIZE, SCREEN_Y_SIZE);

    //设置窗口位无边框
    this->setWindowFlags(Qt::FramelessWindowHint);

    //设置背景式样
    this->setStyleSheet("QMainWindow {border-image: url(:/background/Resource/background/bg.png)}");
}

/* ComboBox初始化 */
void MainWindow::comboBoxInit(void)
{
    QStringList baudList;           //波特率
    QStringList parityList;         //校验位
    QStringList dataBitsList;       //数据位
    QStringList stopBitsList;       //停止位

    //添加波特率
    baudList<<"50"<<"75"<<"100"<<"134"<<"150"<<"200"<<"300"
               <<"600"<<"1200"<<"1800"<<"2400"<<"4800"<<"9600"
              <<"14400"<<"19200"<<"38400"<<"56000"<<"57600"
             <<"76800"<<"115200"<<"128000"<<"256000";
    ui->comboBox_baud->addItems(baudList);
    ui->comboBox_baud->setCurrentIndex(19);

    //添加数据位
    dataBitsList<<"5"<<"6"<<"7"<<"8";
    ui->comboBox_data_bit->addItems(dataBitsList);
    ui->comboBox_data_bit->setCurrentIndex(3);

    //添加奇偶校验
    parityList<<"无校验"<<"奇校验"<<"偶校验";
    ui->comboBox_parity_check->addItems(parityList);
    ui->comboBox_parity_check->setCurrentIndex(0);

    //添加停止位
    stopBitsList<<"1"<<"1.5"<<"2";
    ui->comboBox_stop_bit->addItems(stopBitsList);
    ui->comboBox_stop_bit->setCurrentIndex(0);
}

/* 初始化串口 */
void MainWindow::portInit(void)
{
    int i = 1;
    currentSerialPort = new QSerialPort(this);
    serialPorts.clear();

    //扫描系统当前的串口设备
    foreach (const QSerialPortInfo &serInfo, QSerialPortInfo::availablePorts())
    {
        qDebug() << i++;
        qDebug() << "Name: " << serInfo.portName();
        qDebug() << "Description: " << serInfo.description();
        qDebug() << "Manufacturer: " << serInfo.manufacturer();
        qDebug() << "Serial Number: " << serInfo.serialNumber();
        qDebug() << "System Location: " << serInfo.systemLocation();
        qDebug() << "\n";

        serialPorts[serInfo.portName()] = serInfo;
        ui->comboBox_port_select->addItem(serInfo.portName());
    }
}

/* 初始化label */
void MainWindow::labelInit(void)
{
    QFont font;
    font.setPixelSize((float)25 / 800 * this->geometry().width());

    //title
    ui->label_title->setStyleSheet("QLabel {color: white}");
    ui->label_title->setFont(font);
    ui->label_title->setAlignment(Qt::AlignCenter);

    font.setPixelSize((float)20 / 800 * this->geometry().width());

    //send
    ui->label_send->setStyleSheet("QLabel {color: white}");
    ui->label_send->setFont(font);
    ui->label_send->setAlignment(Qt::AlignCenter);

    //receive
    ui->label_receive->setStyleSheet("QLabel {color: white}");
    ui->label_receive->setFont(font);
    ui->label_receive->setAlignment(Qt::AlignCenter);

    font.setPixelSize((float)15 / 800 * this->geometry().width());

    //port_select
    ui->label_port_select->setStyleSheet("QLabel {color: white}");
    ui->label_port_select->setFont(font);
    ui->label_port_select->setAlignment(Qt::AlignLeft);

    //stop_bit
    ui->label_stop_bit->setStyleSheet("QLabel {color: white}");
    ui->label_stop_bit->setFont(font);
    ui->label_stop_bit->setAlignment(Qt::AlignLeft);

    //baud
    ui->label_baud->setStyleSheet("QLabel {color: white}");
    ui->label_baud->setFont(font);
    ui->label_baud->setAlignment(Qt::AlignLeft);

    //parity_check
    ui->label_parity_check->setStyleSheet("QLabel {color: white}");
    ui->label_parity_check->setFont(font);
    ui->label_parity_check->setAlignment(Qt::AlignLeft);

    //data_bit
    ui->label_data_bit->setStyleSheet("QLabel {color: white}");
    ui->label_data_bit->setFont(font);
    ui->label_data_bit->setAlignment(Qt::AlignLeft);
}

/* 初始化textPlain */
void MainWindow::textPlainInit(void)
{
    QFont font;
    font.setPixelSize((float)15 / 800 * this->geometry().width());
    font.setBold(true);

    //设置成透明
    QPalette pl = ui->textEdit_receive->palette();
    pl.setBrush(QPalette::Base,QBrush(QColor(255,0,0,0)));
    ui->textEdit_receive->setPalette(pl);
    pl = ui->textEdit_send->palette();
    pl.setBrush(QPalette::Base,QBrush(QColor(255,0,0,0)));
    ui->textEdit_send->setPalette(pl);

    //设置字体, 式样
    ui->textEdit_send->setFont(font);
    ui->textEdit_send->setStyleSheet("QTextEdit {color: white}");
    ui->textEdit_receive->setFont(font);
    ui->textEdit_receive->setStyleSheet("QTextEdit {color: white}");
}

/* 按钮初始化 */
void MainWindow::btnInit(void)
{
    //exit
    ui->btn_exit->setStyleSheet(BUTTON_EXIT);

    //槽
    connect(ui->btn_port_op, &QPushButton::clicked, this, &MainWindow::do_btn_port_op_slot);             //串口操作(打开, 关闭)
    connect(ui->btn_send, &QPushButton::clicked, this, &MainWindow::do_btn_send_slot);                   //发送数据
    connect(ui->btn_exit, &QPushButton::clicked, this, &MainWindow::do_btn_exit_slot);                   //退出
    connect(ui->btn_clear_receive, &QPushButton::clicked, this, &MainWindow::do_btn_clear_receive_slot); //清除接收
    connect(ui->btn_clear_send, &QPushButton::clicked, this, &MainWindow::do_btn_clear_send_slot);       //清除发送
}

/* 读取数据槽函数 */
void MainWindow::do_read_port_data_slot(void)
{
    QByteArray rev_byte_array;
    rev_byte_array = currentSerialPort->readAll();
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString rev_str = codec->toUnicode(rev_byte_array);
    qDebug() << rev_str;
    ui->textEdit_receive->append(rev_str);
}

/* 发送数据按钮槽函数 */
void MainWindow::do_btn_send_slot(void)
{
    QString send_content = QString("123");
    if (currentSerialPort->isOpen() == true) {
        currentSerialPort->write(send_content.toLocal8Bit());
    } else {
        QMessageBox::warning(this, "警告", "串口没有打开 !");
    }
}

/* 关闭窗口槽函数 */
void MainWindow::do_btn_exit_slot(void)
{
    this->close();
}

/* 清除接收数据槽函数 */
void MainWindow::do_btn_clear_receive_slot(void)
{
    qDebug() << "do_btn_clear_receive_slot";
    ui->textEdit_receive->clear();
}

/* 清除发送数据槽函数 */
void MainWindow::do_btn_clear_send_slot(void)
{
    qDebug() << "do_btn_clear_send_slot";
    ui->textEdit_send->clear();
}

/* 串口操作按钮槽函数 */
void MainWindow::do_btn_port_op_slot(bool isPressed)
{
    if (isPressed == true) {

        //先把串口关闭, 然后再操作
        currentSerialPort->close();

        //获取用户设置的参数
        QString current_port_select = ui->comboBox_port_select->currentText();
        QString current_baud = ui->comboBox_baud->currentText();
        QString current_stop_bit = ui->comboBox_stop_bit->currentText();
        QString current_data_bit = ui->comboBox_data_bit->currentText();
        QString current_parity_check = ui->comboBox_parity_check->currentText();
        qDebug() << current_baud << current_stop_bit << current_data_bit
                 << current_parity_check << current_port_select;

        //检查是不是没有可用串口
        if (current_port_select.isEmpty()) {
            QMessageBox::warning(this, "警告", "没有可用串口 !");
            return;
        }

        //设置
        currentSerialPort->setPort(serialPorts[current_port_select]);
        if (currentSerialPort->open(QIODevice::ReadWrite))
        {
            qDebug() << QString("%1 opened").arg(current_port_select);

            //设置数据帧格式
            currentSerialPort->setBaudRate(current_baud.toInt());           //波特率

            if (current_parity_check == "奇校验") {                          //奇偶校验
                currentSerialPort->setParity(QSerialPort::OddParity);
            } else if (current_parity_check == "偶校验") {
                currentSerialPort->setParity(QSerialPort::EvenParity);
            } else {
                currentSerialPort->setParity(QSerialPort::NoParity);
            }

            currentSerialPort->setDataBits(QSerialPort::DataBits(current_data_bit.toInt()));  //数据位设置

            if (current_stop_bit == "1") {                                  //停止位设置
                currentSerialPort->setStopBits(QSerialPort::OneStop);
            } else if (current_stop_bit == "1.5") {
                currentSerialPort->setStopBits(QSerialPort::OneAndHalfStop);
            } else {
                currentSerialPort->setStopBits(QSerialPort::TwoStop);
            }

            currentSerialPort->setFlowControl(QSerialPort::NoFlowControl);  //不要流控

            currentSerialPort->clearError();
            currentSerialPort->clear();

            //连接槽函数
            connect(currentSerialPort, SIGNAL(readyRead()), this, SLOT(do_read_port_data_slot()));
        }
        else
        {
            qDebug() << QString("%1 open failed").arg(current_port_select);
            QMessageBox::warning(this, "警告", QString("%1打开失败 !").arg(current_port_select));
        }
    }
    else
    {
        qDebug() << "close port";
        currentSerialPort->close();
    }
}
