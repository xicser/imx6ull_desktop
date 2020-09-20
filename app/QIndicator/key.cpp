#include "key.h"
#include "indicator.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <QDebug>

Key::Key()
{

}

/* 设备文件句柄 */
s32 Key::fd;

/* key驱动信号处理 */
void Key::doKeySignal(s32 signum)
{
    signum = signum;
    u8 key_val;

    read(fd, &key_val, 1);
    emit key_press_signal(key_val);
}

/* key异步通知初始化 */
void Key::keySignalInit(void)
{
    pid_t pid;
    u32 Oflags;

    //注册信号处理函数
    signal(SIGIO, Key::doKeySignal);

    fd = open("/dev/akey", O_RDWR);
    if (Key::fd < 0) {
        qDebug() << "key can't open!\n";
    }

    pid =  getpid();
    qDebug()  << "PID =" << pid;
    fcntl(Key::fd, F_SETOWN,pid);               //把进程ID告诉内核
    Oflags = fcntl(Key::fd, F_GETFL);
    fcntl(Key::fd, F_SETFL, Oflags | FASYNC);   //驱动程序中的.fasync()会被调用
}
