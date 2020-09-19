#ifndef QINDICATORMAINWINDOW_H
#define QINDICATORMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class QIndicatorMainWindow;
}

class QIndicatorMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit QIndicatorMainWindow(QWidget *parent = 0);
    ~QIndicatorMainWindow();

private:
    Ui::QIndicatorMainWindow *ui;

     //蜂鸣器控制按钮组


    void mainWindowtInit(void);                      //主界面初始化
};

#endif // QINDICATORMAINWINDOW_H
