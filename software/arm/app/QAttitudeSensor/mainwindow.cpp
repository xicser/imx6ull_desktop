#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mainWindowInit();
    labelInit();
    meterInit();

    icm20608 = new Icm20608();
    connect(icm20608, &Icm20608::icm20608_data_signal, this, &MainWindow::do_icm20608_data_slot); //绑定槽
    icm20608->start();

    //设置开场, 退场动画
    animation = new Animation(this);
    animation->setGeometry(this->geometry().width() - 48, 0, 48, 48); //设置Animation的位置
    animation->setAnimationObject(this);                              //设置动画对象为MainWindow(开始动画)
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
    init.maxValue = 2;
    init.minValue = -2;
    init.value = 0;
    init.unit = "g";
    init.scaleMajor = 10;
    init.scaleMinor = 5;
    init.precision = 2;
    init.startAngle = 30;
    init.endAngle = 30;
    init.title = "Y-accel";
    init.size_x = 255;
    init.size_y = 189;
    y_AccelMeter = new Meter(ui->label_accel_y, &init);

    //z轴加速度仪表
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
    init.title = "Z-accel";
    init.size_x = 255;
    init.size_y = 189;
    z_AccelMeter = new Meter(ui->label_accel_z, &init);

    //x轴角速度仪表
    init.background = Qt::black;
    init.foreground = Qt::green;
    init.maxValue = 200;
    init.minValue = -200;
    init.value = 0;
    init.unit = "dps";
    init.scaleMajor = 10;
    init.scaleMinor = 5;
    init.precision = 2;
    init.startAngle = 30;
    init.endAngle = 30;
    init.title = "X-gyro";
    init.size_x = 255;
    init.size_y = 189;
    x_GyroMeter = new Meter(ui->label_gyro_x, &init);

    //x轴角速度仪表
    init.background = Qt::black;
    init.foreground = Qt::green;
    init.maxValue = 200;
    init.minValue = -200;
    init.value = 0;
    init.unit = "dps";
    init.scaleMajor = 10;
    init.scaleMinor = 5;
    init.precision = 2;
    init.startAngle = 30;
    init.endAngle = 30;
    init.title = "Y-gyro";
    init.size_x = 255;
    init.size_y = 189;
    y_GyroMeter = new Meter(ui->label_gyro_y, &init);

    //z轴角速度仪表
    init.background = Qt::black;
    init.foreground = Qt::green;
    init.maxValue = 200;
    init.minValue = -200;
    init.value = 0;
    init.unit = "dps";
    init.scaleMajor = 10;
    init.scaleMinor = 5;
    init.precision = 2;
    init.startAngle = 30;
    init.endAngle = 30;
    init.title = "Z-gyro";
    init.size_x = 255;
    init.size_y = 189;
    z_GyroMeter = new Meter(ui->label_gyro_z, &init);
}

/* 处理icm20608数据槽函数 */
void MainWindow::do_icm20608_data_slot(Icm20608_Act_Data_t *data)
{
//    printf("do_icm20608_data_slot ---> gyro(x, y, z) = (%.3f %.3f %.3f),  accel(x, y, z) = (%.3f %.3f %.3f),  temp = (%.3f)\n",
//        data->gyro_x_act, data->gyro_y_act, data->gyro_z_act,
//        data->accel_x_act, data->accel_y_act, data->accel_z_act,
//        data->temp_act);

    x_AccelMeter->setNeedle(data->accel_x_act);
    y_AccelMeter->setNeedle(data->accel_y_act);
    z_AccelMeter->setNeedle(data->accel_z_act);

    x_GyroMeter->setNeedle(data->gyro_x_act);
    y_GyroMeter->setNeedle(data->gyro_y_act);
    z_GyroMeter->setNeedle(data->gyro_z_act);
}
