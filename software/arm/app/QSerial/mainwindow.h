#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QTextCodec>
#include <QDebug>
#include "config.h"
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
    Animation *animation;                       //进场, 退场动画
    QSerialPort *currentSerialPort;             //当前使用的串口
    QMap<QString, QSerialPortInfo> serialPorts; //串口集合

    void mainWindowInit(void);                  //初始化主窗口
    void labelInit(void);                       //初始化label
    void textPlainInit(void);                   //初始化textPlain
    void btnInit(void);                         //按钮初始化
    void comboBoxInit(void);                    //ComboBox初始化
    void portInit(void);                        //串口初始化

private slots:
    void do_read_port_data_slot(void);          //读取数据槽函数
    void do_btn_send_slot(void);                //发送数据按钮槽函数
    void do_btn_clear_receive_slot(void);       //清除接收数据槽函数
    void do_btn_clear_send_slot(void);          //清除发送数据槽函数
    void do_btn_port_op_slot(bool isPressed);   //串口操作按钮槽函数
};

#endif // MAINWINDOW_H
