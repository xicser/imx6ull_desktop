#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "meter.h"
#include "config.h"
#include "icm20608.h"
#include "animation.h"

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
    Animation *animation;            //进场, 退场动画

    Icm20608 *icm20608;              //icm20608

    Meter *x_AccelMeter;             //z轴加速度仪表
    Meter *y_AccelMeter;             //y轴加速度仪表
    Meter *z_AccelMeter;             //z轴加速度仪表

    Meter *x_GyroMeter;              //z轴角速度仪表
    Meter *y_GyroMeter;              //y轴角速度仪表
    Meter *z_GyroMeter;              //z轴角速度仪表

    void mainWindowInit(void);       //初始化主窗口
    void labelInit(void);            //初始化label
    void meterInit(void);            //仪表初始化

private slots:
    void do_icm20608_data_slot(Icm20608_Act_Data_t *data);  //处理icm20608数据槽函数
};

#endif // MAINWINDOW_H
