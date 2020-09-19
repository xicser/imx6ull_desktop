#include "qindicatormainwindow.h"
#include "ui_qindicatormainwindow.h"
#include <QDesktopWidget>

QIndicatorMainWindow::QIndicatorMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QIndicatorMainWindow)
{
    ui->setupUi(this);

    mainWindowtInit();                          //初始化主界面

}

QIndicatorMainWindow::~QIndicatorMainWindow()
{
    delete ui;
}

/* 主界面初始化 */
void QIndicatorMainWindow::mainWindowtInit(void)
{
    //设置窗口位置
    this->setGeometry(0, 0, QApplication::desktop()->screenGeometry().width(),
                      QApplication::desktop()->screenGeometry().height());

    //设置窗口位无边框
    this->setWindowFlags(Qt::FramelessWindowHint);

    //设置式样
    this->setStyleSheet("QMainWindow {border-image: url(:/background/Resource/background/bg)}");
}
