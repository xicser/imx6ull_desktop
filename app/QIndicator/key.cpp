#include "key.h"
#include "config.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <QDebug>


Key::Key(QThread *parent) : QThread(parent)
{

}

Key::~Key()
{
    close(fd);
}

void Key::run(void)
{
    s32 poll_ret;
    u8 key_val;
    struct pollfd pfd;

    fd = open("/dev/akey", O_RDWR | O_NONBLOCK);
    if (fd < 0) {
        qDebug() << "key can't open!\n";
    }

    pfd.fd = fd;
    pfd.events = POLLIN;
    while (1) {
        poll_ret = poll(&pfd, 1, 5000);
        if(poll_ret == 0) {
            // qDebug() << "time out(%d)\n";
        } else if (poll_ret < 0) {
            qDebug() << "poll err(%d)\n";
        } else {
            if (pfd.revents | POLLIN) {
                read(fd, &key_val, 1);
                emit key_press_signal(key_val);  //发射信号(按键值)
            }
        }
    }
}
