#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mainWindowInit();
    labelInit();
    btnInit();
    meterInit();

    icm20608 = new Icm20608();
    connect(icm20608, &Icm20608::icm20608_data_signal, this, &MainWindow::do_icm20608_data_slot); //绑定槽
    icm20608->start();
}

MainWindow::~MainWindow()
{
    icm20608->terminate();
    delete icm20608;
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

/* 初始化label */
void MainWindow::labelInit(void)
{
    QFont font;
    font.setPixelSize((float)25 / 800 * this->geometry().width());

    //title
    ui->label_title->setStyleSheet("QLabel {color: white}");
    ui->label_title->setFont(font);
    ui->label_title->setAlignment(Qt::AlignCenter);

    font.setPixelSize((float)18 / 800 * this->geometry().width());

    //accel
    ui->label_accel->setStyleSheet("QLabel {color: white}");
    ui->label_accel->setFont(font);
    ui->label_accel->setAlignment(Qt::AlignLeft);

    //gyro
    ui->label_gyro->setStyleSheet("QLabel {color: white}");
    ui->label_gyro->setFont(font);
    ui->label_gyro->setAlignment(Qt::AlignLeft);
}

/* 按钮初始化 */
void MainWindow::btnInit(void)
{
    //exit
    ui->btn_exit->setStyleSheet(BUTTON_EXIT);
    connect(ui->btn_exit, &QPushButton::clicked, this, &MainWindow::do_btn_exit_slot);                   //退出
}

/* 仪表初始化 */
void MainWindow::meterInit(void)
{
    Meter_Init_Param_t init;

    //x轴加速度仪表
    init.background = Qt::black;
    init.foreground = Qt::green;
    init.maxValue = 2;
    init.minValue = -2;
    init.value = 0;
    init.unit = "g";
    init.scaleMajor = 10;
    init.scaleMinor = 5;
    init.precision = 2;
    init.startAngle = 30;
    init.endAngle = 30;
    init.title = "X-accel";
    init.size_x = 255;
    init.size_y = 189;
    x_AccelMeter = new Meter(ui->label_accel_x, &init);

    //y轴加速度仪表
    init.background = Qt::black;
    init.foreground = Qt::green;
    init.maxValue = 100;
    init.minValue = 0;
    init.value = 0;
    init.unit = "km/h";
    init.scaleMajor = 10;
    init.scaleMinor = 10;
    init.precision = 0;
    init.startAngle = 36;
    init.endAngle = 36;
    init.title = "Speed Meter";
    init.size_x = 255;
    init.size_y = 189;
    y_AccelMeter = new Meter(ui->label_accel_y, &init);

    //z轴加速度仪表
    init.background = Qt::black;
    init.foreground = Qt::green;
    init.maxValue = 100;
    init.minValue = 0;
    init.value = 0;
    init.unit = "km/h";
    init.scaleMajor = 10;
    init.scaleMinor = 10;
    init.precision = 0;
    init.startAngle = 36;
    init.endAngle = 36;
    init.title = "Speed Meter";
    init.size_x = 255;
    init.size_y = 189;
    z_AccelMeter = new Meter(ui->label_accel_z, &init);

}

/* 关闭窗口槽函数 */
void MainWindow::do_btn_exit_slot(void)
{
    this->close();
}

/* 处理icm20608数据槽函数 */
void MainWindow::do_icm20608_data_slot(Icm20608_Act_Data_t *data)
{
    printf("do_icm20608_data_slot ---> gyro(x, y, z) = (%.3f %.3f %.3f),  accel(x, y, z) = (%.3f %.3f %.3f),  temp = (%.3f)\n",
        data->gyro_x_act, data->gyro_y_act, data->gyro_z_act,
        data->accel_x_act, data->accel_y_act, data->accel_z_act,
        data->temp_act);

    x_AccelMeter->setNeedle(data->accel_x_act);

}
