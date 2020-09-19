#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "indicator.h"
#include <QDesktopWidget>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mainWindowtInit();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/* 主界面初始化 */
void MainWindow::mainWindowtInit(void)
{
    //设置窗口位置
    this->setGeometry(0, 0, QApplication::desktop()->screenGeometry().width(),
                      QApplication::desktop()->screenGeometry().height());

    //设置窗口位无边框
    this->setWindowFlags(Qt::FramelessWindowHint);

    //设置式样
    this->setStyleSheet("QMainWindow {border-image: url(:/background/Resource/background/bg)}");
}

/* 初始化槽 */
void MainWindow::slotsInit(void)
{
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()));
}

void MainWindow::on_pushButton_clicked()
{
    static bool on_off = false;
    qDebug() << on_off;
    if (on_off == true) {
        Indicator::indicatorControl(INDICATOR_LED, INDICATOR_ON);
        on_off = false;
    } else {
        Indicator::indicatorControl(INDICATOR_LED, INDICATOR_OFF);
        on_off = true;
    }

}
