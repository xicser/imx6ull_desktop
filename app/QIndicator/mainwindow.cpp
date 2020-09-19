#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "indicator.h"
#include "config.h"
#include <QDesktopWidget>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{    
    ui->setupUi(this);

    //默认情况下, 先关闭
    Indicator::indicatorControl(INDICATOR_LED, INDICATOR_OFF);
    Indicator::indicatorControl(INDICATOR_BEEP, INDICATOR_OFF);

    mainWindowtInit();
    buttonInit();
    timerInit();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/* 主界面初始化 */
void MainWindow::mainWindowtInit(void)
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

/* 按钮初始化 */
void MainWindow::buttonInit(void)
{
    bg_led = new QButtonGroup(this);   //led按键组
    bg_beep = new QButtonGroup(this);  //beep按键组

    bg_led->addButton(ui->radioButton_led_off);
    bg_led->addButton(ui->radioButton_led_on);
    bg_led->addButton(ui->radioButton_led_blink);

    bg_beep->addButton(ui->radioButton_beep_off);
    bg_beep->addButton(ui->radioButton_beep_on);
    bg_beep->addButton(ui->radioButton_beep_blink);

    connect(bg_led, SIGNAL(buttonClicked(int)), this, SLOT(do_led_slot(int)));
    connect(bg_beep, SIGNAL(buttonClicked(int)), this, SLOT(do_beep_slot(int)));
}

/* 定时器初始化 */
void MainWindow::timerInit(void)
{
    led_blink_timer = new QTimer(this);
    beep_blink_timer = new QTimer(this);

    connect(led_blink_timer, &QTimer::timeout, this, &MainWindow::do_led_timer_slot);
    connect(beep_blink_timer, &QTimer::timeout, this, &MainWindow::do_beep_timer_slot);
}

/* led定时超时槽函数 */
void MainWindow::do_led_timer_slot(void)
{
    qDebug() << "led blink";
    static bool toggle = true;
    if (toggle == true) {
        toggle = false;
        Indicator::indicatorControl(INDICATOR_LED, INDICATOR_OFF);
    } else {
        toggle = true;
        Indicator::indicatorControl(INDICATOR_LED, INDICATOR_ON);
    }
}

/* beep定时超时槽函数 */
void MainWindow::do_beep_timer_slot(void)
{
    qDebug() << "beep blink";
    static bool toggle = true;
    if (toggle == true) {
        toggle = false;
        Indicator::indicatorControl(INDICATOR_BEEP, INDICATOR_OFF);
    } else {
        toggle = true;
        Indicator::indicatorControl(INDICATOR_BEEP, INDICATOR_ON);
    }
}

/* led控制按钮槽函数 */
void MainWindow::do_led_slot(int id)
{
    qDebug() << "led =" << id;
    switch (id) {
        case -2: {
            led_blink_timer->stop();
            Indicator::indicatorControl(INDICATOR_LED, INDICATOR_OFF);
            break;
        }
        case -3: {
            led_blink_timer->stop();
            Indicator::indicatorControl(INDICATOR_LED, INDICATOR_ON);
            break;
        }
        case -4: {
            led_blink_timer->start(300);
            break;
        }
        default: break;
    }
}
/* beep控制按钮槽函数 */
void MainWindow::do_beep_slot(int id)
{
    qDebug() << "beep =" << id;
    switch (id) {
        case -2: {
            beep_blink_timer->stop();
            Indicator::indicatorControl(INDICATOR_BEEP, INDICATOR_OFF);
            break;
        }
        case -3: {
            beep_blink_timer->stop();
            Indicator::indicatorControl(INDICATOR_BEEP, INDICATOR_ON);
            break;
        }
        case -4: {
            beep_blink_timer->start(300);
            break;
        }
        default: break;
    }
}

