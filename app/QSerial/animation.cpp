#include "animation.h"
#include <QPropertyAnimation>
#include <QProcess>

Animation::Animation(QWidget *parent) : QWidget(parent)
{
    setMaximumSize(QSize(48, 48));

    btn_exit = new QPushButton(this);
    btn_exit->setGeometry(0, 0, 48, 48);
    btn_exit->setStyleSheet("QPushButton {border-image:url(:/icon/Resource/icon/menu.png);}"
                            "QPushButton:hover{border-image:url(:/icon/Resource/icon/menu_clicked.png);}"
                            "QPushButton:pressed{border-image:url(:/icon/Resource/icon/menu.png);}");
    btn_exit->setFocusPolicy(Qt::NoFocus);

    this->show();

    //按钮的槽
    connect(btn_exit, SIGNAL(clicked()), this, SLOT(do_exit_annimation_slot()));
}

/* 设置动画对象 */
void Animation::setAnimationObject(QMainWindow *obj)
{
    animationObject = obj;

    //进场动画
    animation_enter = new QPropertyAnimation(animationObject, "geometry", this);

    //动画时长0.5s
    animation_enter->setDuration(500);

    //从左下角开始, 到覆盖整幅屏幕为止
    animation_enter->setStartValue(QRect(0, animationObject->geometry().height(), 0, 0));
    animation_enter->setEndValue(QRect(0, 0, animationObject->geometry().width(), animationObject->geometry().height()));
    animation_enter->start();

    //更新动画的槽
    connect(animation_enter, SIGNAL(finished()), this, SLOT(do_update_slot()));
}

void Animation::do_exit_annimation_slot(void)
{
    //退场动画
    animation_exit = new QPropertyAnimation(animationObject, "geometry", this);

    //动画时长是0.5s
    animation_exit->setDuration(500);

    //从覆盖整幅屏幕开始, 到右上角为止
    animation_exit->setStartValue(QRect(0, 0, animationObject->geometry().width(), animationObject->geometry().height()));
    animation_exit->setEndValue(QRect(animationObject->geometry().width(), 0, 0, 0));
    animation_exit->start();

    //连接槽, 用于在动画完成后关闭MainWindow
    connect(animation_exit, SIGNAL(finished()), this, SLOT(do_close_slot()));
}

/* 关闭槽函数 */
void Animation::do_close_slot(void)
{
    animationObject->close();
}

/* 重绘槽函数 */
void Animation::do_update_slot(void)
{
    animationObject->repaint();
}
