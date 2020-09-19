#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QButtonGroup>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void do_led_slot(int);
    void do_beep_slot(int);
    void do_led_timer_slot(void);
    void do_beep_timer_slot(void);

private:
    Ui::MainWindow *ui;
    QButtonGroup *bg_led;                            //led按键组
    QButtonGroup *bg_beep;                           //beep按键组
    QTimer *led_blink_timer, *beep_blink_timer;      //闪烁(滴答)控制用定时器

    void mainWindowtInit(void);                      //主界面初始化
    void buttonInit(void);                           //按钮初始化
    void timerInit(void);                            //定时器初始化
};

#endif // MAINWINDOW_H
