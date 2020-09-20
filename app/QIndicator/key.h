#ifndef KEY_H
#define KEY_H

#include "config.h"
#include <QThread>

class Key : public QThread
{
    Q_OBJECT
public:
    explicit Key(QThread *parent = 0);

signals:
    void key_press_signal(u8 key_status);         //按钮信号

protected:
    void run(void);
};


#endif // KEY_H
