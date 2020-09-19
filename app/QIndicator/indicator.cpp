#include <QDebug>

#include "indicator.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define u32   unsigned int
#define u16   unsigned short
#define u8    unsigned char
#define s32   signed   int
#define s16   signed   short
#define s8    signed   char

Indicator::Indicator()
{

}

/* 控制led或者beep */
void Indicator::indicatorControl(Indicator_t which, Status_t status)
{
    u8 buffer;
    s32 fd, ret;

    //状态设置
    if (status == INDICATOR_ON) {
        buffer = 1;
    } else if (status == INDICATOR_OFF) {
        buffer = 0;
    } else {
        return;
    }

    //设备选择
    if (which == INDICATOR_LED) {
        fd = open("/dev/alpha_i0", O_RDWR);
    } else if (which == INDICATOR_BEEP) {
        fd = open("/dev/alpha_i1", O_RDWR);
    } else {
        return;
    }

    ret = write(fd, &buffer, sizeof(buffer));
    if (ret < 0) {
        qDebug() << "control error";
    }
    close(fd);
}
