#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "meter.h"
#include "config.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    Meter *x_Accel;                  //z轴加速度仪表
    Meter *y_Accel;                  //y轴加速度仪表
    Meter *z_Accel;                  //z轴加速度仪表

    Meter *x_Gyro;                   //z轴角速度仪表
    Meter *y_Gyro;                   //y轴角速度仪表
    Meter *z_Gyro;                   //z轴角速度仪表

    void mainWindowInit(void);       //初始化主窗口
    void labelInit(void);            //初始化label
    void btnInit(void);              //按钮初始化
    void meterInit(void);            //仪表初始化

private slots:
    void do_btn_exit_slot(void);     //关闭窗口槽函数
};

#endif // MAINWINDOW_H
