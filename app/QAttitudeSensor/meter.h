#ifndef METER_H
#define METER_H

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QColor>
#include <QDebug>
#include <QTimer>
#include <complex>

class Meter : public QWidget
{
    Q_OBJECT
public:
    explicit Meter(QWidget *parent = 0);
    ~Meter();

protected:
    void paintEvent(QPaintEvent *);
    void drawCrown(QPainter *painter);
    void drawBackground(QPainter *painter);
    void drawScale(QPainter *painter);
    void drawScaleNum(QPainter *painter);
    void drawTitle(QPainter *painter);
    void drawIndicator(QPainter *painter);
    void drawNumericValue(QPainter *painter);

private:
    QString m_units;                    //计量单位
    QString m_title;                    //标题

    QColor m_background;                //背景颜色
    QColor m_foreground;                //刻度颜色

    int m_maxValue;                     //刻度表上的最大值
    int m_minValue;                     //刻度表上的最小值

    int m_startAngle;                   //起始角度(相对于竖直方向)
    int m_endAngle;                     //终止角度(相对于竖直方向)

    int m_scaleMajor;                   //大格子的个数
    int m_scaleMinor;                   //两个大格子之间, 小格子的个数

    double m_value;                     //指针位置

    int m_precision;                    //显示的数字精度(小数点位数)
};

#endif // METER_H
