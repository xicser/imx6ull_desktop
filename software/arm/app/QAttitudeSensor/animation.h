#ifndef ANIMATION_H
#define ANIMATION_H

#include <QPushButton>
#include <QWidget>
#include <QObject>
#include <QMainWindow>
#include <QPropertyAnimation>

class Animation : public QWidget
{
    Q_OBJECT

public:
    explicit Animation(QWidget *parent = 0);
    void setAnimationObject(QMainWindow *obj);

public slots:
    void do_close_slot(void);
    void do_exit_annimation_slot(void);
    void do_update_slot(void);

private:
    QPushButton         *btn_exit;                  //退出按钮
    QMainWindow         *animationObject;           //动画对象(MainWindow)
    QPropertyAnimation  *animation_enter;           //进场动画
    QPropertyAnimation  *animation_exit;            //退场动画
};

#endif // ENTEREXITANIMATION_H
