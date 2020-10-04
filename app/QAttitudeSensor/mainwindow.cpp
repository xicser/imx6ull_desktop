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
    //创建仪表
    z_Accel = new Meter();
    z_Accel->setParent(ui->label_accel_x);
    z_Accel->show();
}


/* 关闭窗口槽函数 */
void MainWindow::do_btn_exit_slot(void)
{
    this->close();
}
