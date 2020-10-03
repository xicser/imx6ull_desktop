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
    QColor m_background;
    QColor m_foreground;

    int m_maxValue;
    int m_minValue;
    int m_startAngle;
    int m_endAngle;

    int m_scaleMajor;
    int m_scaleMinor;
    double m_value;
    int m_precision;
    QTimer *m_updateTimer;
    QString m_units;
    QString m_title;

private slots:
    void UpdateAngle();
};

#endif // METER_H
