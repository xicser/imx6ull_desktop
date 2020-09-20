#ifndef KEY_H
#define KEY_H

#include "config.h"
#include <QObject>

class Key : public QObject
{
    Q_OBJECT
public:
    Key();
    static void doKeySignal(s32 signum);          //key驱动信号处理
    static void keySignalInit(void);              //key异步通知初始化

private:
    static s32 fd;                                //设备文件句柄

signals:
    void key_press_signal(u8 key_status);         //按钮信号
};

#endif // KEY_H
