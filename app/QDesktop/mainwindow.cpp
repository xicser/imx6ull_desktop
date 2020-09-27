#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "config.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mainWindowInit();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/* 主窗口初始化 */
void MainWindow::mainWindowInit(void)
{
    //this->setGeometry(0,0,QApplication::desktop()->screenGeometry().width(),QApplication::desktop()->screenGeometry().height());
    this->setGeometry(0, 0, SCREEN_X_SIZE, SCREEN_Y_SIZE);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setStyleSheet("QMainWindow {border-image: url(:/background/Resource/background/bg.png)}");
}
