#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "indicator.h"
#include "key.h"
#include "config.h"
#include <QDesktopWidget>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{    
    ui->setupUi(this);

    //new一个按键
    key = new Key();
    key->keySignalInit(); //将该进程绑定到按键驱动的异步通知机制上
    connect(key, &Key::key_press_signal, this, &MainWindow::do_key_press_signal_slot); //绑定槽

    //默认情况下, 先关闭
    Indicator::indicatorControl(INDICATOR_LED, INDICATOR_OFF);
    Indicator::indicatorControl(INDICATOR_BEEP, INDICATOR_OFF);

    mainWindowtInit();
    buttonInit();
    labelInit();
    timerInit();
}

MainWindow::~MainWindow()
{
    delete key;
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
    QFont font;
    bg_led = new QButtonGroup(this);   //led按键组
    bg_beep = new QButtonGroup(this);  //beep按键组

    font.setPixelSize((float)28 / 800 * this->geometry().width());
    QString str = QString::number((float)36 / 800 * this->geometry().width());

    //led
    ui->radioButton_led_off->setStyleSheet("QRadioButton::indicator {width: " + str + "px; height: " + str + "px;}"
            "QRadioButton::indicator::unchecked {border-image: url(:/icon/Resource/icon/btn_unchecked.png);}"
            "QRadioButton::indicator::checked {border-image: url(:/icon/Resource/icon/btn_checked.png);}"
            "QRadioButton {color: white; outline: 0px;}");
    ui->radioButton_led_off->setFont(font);

    ui->radioButton_led_on->setStyleSheet("QRadioButton::indicator {width: " + str + "px; height: " + str + "px;}"
            "QRadioButton::indicator::unchecked {border-image: url(:/icon/Resource/icon/btn_unchecked.png);}"
            "QRadioButton::indicator::checked {border-image: url(:/icon/Resource/icon/btn_checked.png);}"
            "QRadioButton {color: white; outline: 0px;}");
    ui->radioButton_led_on->setFont(font);

    ui->radioButton_led_blink->setStyleSheet("QRadioButton::indicator {width: " + str + "px; height: " + str + "px;}"
            "QRadioButton::indicator::unchecked {border-image: url(:/icon/Resource/icon/btn_unchecked.png);}"
            "QRadioButton::indicator::checked {border-image: url(:/icon/Resource/icon/btn_checked.png);}"
            "QRadioButton {color: white; outline: 0px;}");
    ui->radioButton_led_blink->setFont(font);

    //beep
    ui->radioButton_beep_off->setStyleSheet("QRadioButton::indicator {width: " + str + "px; height: " + str + "px;}"
            "QRadioButton::indicator::unchecked {border-image: url(:/icon/Resource/icon/btn_unchecked.png);}"
            "QRadioButton::indicator::checked {border-image: url(:/icon/Resource/icon/btn_checked.png);}"
            "QRadioButton {color: white; outline: 0px;}");
    ui->radioButton_beep_off->setFont(font);

    ui->radioButton_beep_on->setStyleSheet("QRadioButton::indicator {width: " + str + "px; height: " + str + "px;}"
            "QRadioButton::indicator::unchecked {border-image: url(:/icon/Resource/icon/btn_unchecked.png);}"
            "QRadioButton::indicator::checked {border-image: url(:/icon/Resource/icon/btn_checked.png);}"
            "QRadioButton {color: white; outline: 0px;}");
    ui->radioButton_beep_on->setFont(font);

    ui->radioButton_beep_blink->setStyleSheet("QRadioButton::indicator {width: " + str + "px; height: " + str + "px;}"
            "QRadioButton::indicator::unchecked {border-image: url(:/icon/Resource/icon/btn_unchecked.png);}"
            "QRadioButton::indicator::checked {border-image: url(:/icon/Resource/icon/btn_checked.png);}"
            "QRadioButton {color: white; outline: 0px;}");
    ui->radioButton_beep_blink->setFont(font);

    //将按钮添加到按钮组
    bg_led->addButton(ui->radioButton_led_off);
    bg_led->addButton(ui->radioButton_led_on);
    bg_led->addButton(ui->radioButton_led_blink);
    bg_beep->addButton(ui->radioButton_beep_off);
    bg_beep->addButton(ui->radioButton_beep_on);
    bg_beep->addButton(ui->radioButton_beep_blink);

    //退出按钮
    ui->btn_exit->setStyleSheet("QPushButton {border-image:url(:/icon/Resource/icon/menu.png);}"
                                "QPushButton:hover{border-image:url(:/icon/Resource/icon/menu_clicked.png);}"
                                "QPushButton:pressed{border-image:url(:/icon/Resource/icon/menu.png);}");
    ui->btn_exit->setFocusPolicy(Qt::NoFocus);

    //槽
    connect(bg_led, SIGNAL(buttonClicked(int)), this, SLOT(do_led_slot(int)));
    connect(bg_beep, SIGNAL(buttonClicked(int)), this, SLOT(do_beep_slot(int)));
    connect(ui->btn_exit, &QPushButton::clicked, this, &MainWindow::do_btn_exit_slot);
}

/* 标签初始化 */
void MainWindow::labelInit(void)
{
    QFont font;
    font.setPixelSize((float)25 / 800 * this->geometry().width());

    //led
    ui->label_led_control->setStyleSheet("QLabel {color: white}");
    ui->label_led_control->setFont(font);
    ui->label_led_control->setAlignment(Qt::AlignCenter);

    //beep
    ui->label_beep_control->setStyleSheet("QLabel {color: white}");
    ui->label_beep_control->setFont(font);
    ui->label_beep_control->setAlignment(Qt::AlignCenter);

    //title
    font.setPixelSize((float)30 / 800 * this->geometry().width());
    ui->label_title->setStyleSheet("QLabel {color: white}");
    ui->label_title->setFont(font);
    ui->label_title->setAlignment(Qt::AlignCenter);

    //面板指示灯
    ui->label_led_indi->setStyleSheet("QLabel {border-image: url(:/icon/Resource/icon/led_off.png)}");
    ui->label_led_indi->setText("");
    ui->label_beep_indi->setStyleSheet("QLabel {border-image: url(:/icon/Resource/icon/beep_off.png)}");
    ui->label_beep_indi->setText("");
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
        ui->label_led_indi->setStyleSheet("QLabel {border-image: url(:/icon/Resource/icon/led_off.png)}");
    } else {
        toggle = true;
        Indicator::indicatorControl(INDICATOR_LED, INDICATOR_ON);
        ui->label_led_indi->setStyleSheet("QLabel {border-image: url(:/icon/Resource/icon/led_on.png)}");
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
        ui->label_beep_indi->setStyleSheet("QLabel {border-image: url(:/icon/Resource/icon/beep_off.png)}");
    } else {
        toggle = true;
        Indicator::indicatorControl(INDICATOR_BEEP, INDICATOR_ON);
        ui->label_beep_indi->setStyleSheet("QLabel {border-image: url(:/icon/Resource/icon/beep_on.png)}");
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
            ui->label_led_indi->setStyleSheet("QLabel {border-image: url(:/icon/Resource/icon/led_off.png)}");
            break;
        }
        case -3: {
            led_blink_timer->stop();
            Indicator::indicatorControl(INDICATOR_LED, INDICATOR_ON);
            ui->label_led_indi->setStyleSheet("QLabel {border-image: url(:/icon/Resource/icon/led_on.png)}");
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
            ui->label_beep_indi->setStyleSheet("QLabel {border-image: url(:/icon/Resource/icon/beep_off.png)}");
            break;
        }
        case -3: {
            beep_blink_timer->stop();
            Indicator::indicatorControl(INDICATOR_BEEP, INDICATOR_ON);
            ui->label_beep_indi->setStyleSheet("QLabel {border-image: url(:/icon/Resource/icon/beep_on.png)}");
            break;
        }
        case -4: {
            beep_blink_timer->start(300);
            break;
        }
        default: break;
    }
}

/* 退出按钮槽函数 */
void MainWindow::do_btn_exit_slot(void)
{
    this->close();
}

/* 处理按键信号的槽 */
void MainWindow::do_key_press_signal_slot(u8 key_value)
{
    qDebug() << key_value;
}
