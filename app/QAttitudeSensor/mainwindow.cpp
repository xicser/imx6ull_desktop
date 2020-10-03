#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //创建仪表
    z_Acceleration = new Meter(this);
    z_Acceleration->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}
