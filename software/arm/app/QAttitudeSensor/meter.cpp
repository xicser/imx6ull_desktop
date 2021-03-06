#include "meter.h"

Meter::Meter(QWidget *parent, Meter_Init_Param_t *init) : QWidget(parent)
{
    m_background = init->background;
    m_foreground = init->foreground;
    m_startAngle = init->startAngle;
    m_endAngle = init->endAngle;
    m_minValue = init->minValue;
    m_maxValue = init->maxValue;
    m_scaleMajor = init->scaleMajor;
    m_scaleMinor = init->scaleMinor;
    m_unit = init->unit;
    m_title = init->title;
    m_precision = init->precision;
    m_value = init->value;

    setWindowFlags(Qt::FramelessWindowHint);     //无窗体
    setAttribute(Qt::WA_TranslucentBackground);  //背景透明
    resize(init->size_x, init->size_y);
}

Meter::~Meter()
{

}

/* 设置指针位置 */
void Meter::setNeedle(double value)
{
    if (m_minValue <= value && value <= m_maxValue) {
        m_value = value;
        this->update();
    }
}

/* 绘图事件 */
void Meter::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);  //使用反锯齿(如果可用)
    painter.translate(width() / 2, height() / 2);   //坐标变换为窗体中心
    int side = qMin(width(), height());
    painter.scale(side / 200.0, side / 200.0);      //比例缩放

    drawCrown(&painter);                            //画表盘边框
    drawScale(&painter);                            //画刻度线
    drawScaleNum(&painter);                         //画刻度数值
    drawTitle(&painter);                            //画单位
    drawNumericValue(&painter);                     //画数字显示
    drawIndicator(&painter);                        //画表针
}

/* 绘制表盘边框 */
void Meter::drawCrown(QPainter *painter)
{
    painter->save();
    int radius = 100;
    QLinearGradient lg1(0, -radius, 0, radius);

    lg1.setColorAt(0, Qt::white); //设置渐变的颜色和路径比例
    lg1.setColorAt(1, Qt::gray);  //只是粗略的颜色, 具体的可以参考RGB颜色查询对照表

    painter->setBrush(lg1);       //创建QBrush对象,把这个渐变对象传递进去：
    painter->setPen(Qt::NoPen);   //边框线无色
    painter->drawEllipse(-radius, -radius, radius << 1, radius << 1);
    painter->setBrush(m_background);
    painter->drawEllipse(-92, -92, 184, 184);
    painter->restore();
}

/* 画刻度线 */
void Meter::drawScale(QPainter *painter)
{
    painter->save();
    painter->rotate(m_startAngle);
    int steps = (m_scaleMajor * m_scaleMinor);                      //相乘后的值是分的最小份数
    double angleStep = (360.0 - m_startAngle - m_endAngle) / steps; //每一个份数的角度

    // painter->setPen(m_foreground); //m_foreground是颜色的设置
    // QPen pen = painter->pen();     //第一种方法
    QPen pen;
    pen.setColor(m_foreground);       //推荐使用第二种方式
    for (int i = 0; i <= steps; i++)
    {
        if (i % m_scaleMinor == 0)    //整数刻度显示加粗
        {
            pen.setWidth(1);          //设置线宽
            painter->setPen(pen);     //使用面向对象的思想, 把画笔关联上画家, 通过画家画出来
            painter->drawLine(0, 62, 0, 72); //两个参数应该是两个坐标值
        }
        else
        {
            pen.setWidth(0);
            painter->setPen(pen);
            painter->drawLine(0, 67, 0, 72);
        }
        painter->rotate(angleStep);
    }
    painter->restore();
}

/* 绘制刻度数字 */
void Meter::drawScaleNum(QPainter *painter)
{
    painter->save();
    painter->setPen(m_foreground);
    //m_startAngle是起始角度, m_endAngle是结束角度, m_scaleMajor在一个量程中分成的刻度数
    double startRad = (270 - m_startAngle) * (3.14 / 180);
    double deltaRad = (360 - m_startAngle - m_endAngle) * (3.14 / 180) / m_scaleMajor;
    double sina,cosa;
    int x, y;
    QFontMetricsF fm(this->font());
    double w, h, tmpVal;
    QString str;

    for (int i = 0; i <= m_scaleMajor; i++)
    {
        sina = sin(startRad - i * deltaRad);
        cosa = cos(startRad - i * deltaRad);
        tmpVal = 1.0 * i *((m_maxValue - m_minValue) / m_scaleMajor) + m_minValue;
        str = QString( "%1" ).arg(tmpVal);              //%1作为占位符arg()函数比起sprintf()来是类型安全的
        w = fm.size(Qt::TextSingleLine,str).width();
        h = fm.size(Qt::TextSingleLine,str).height();
        x = 82 * cosa - w / 2;
        y = -82 * sina + h / 4;
        painter->drawText(x, y, str);                   //函数的前两个参数是显示的坐标位置, 后一个是显示的内容，是字符类型""
    }
    painter->restore();
}

/* 绘制标题 */
void Meter::drawTitle(QPainter *painter)
{
    painter->save();
    painter->setPen(m_foreground);
    QString str(m_title);                               //显示仪表的功能
    QFontMetricsF fm(this->font());
    double w = fm.size(Qt::TextSingleLine,str).width();
    painter->drawText(-w / 2, -30, str);
    painter->restore();
}

/* 绘制显示的数字 */
void Meter::drawNumericValue(QPainter *painter)
{
    QString str = QString("%1 %2").arg(m_value, 0, 'f', m_precision).arg(m_unit);
    QFontMetricsF fm(font());
    double w = fm.size(Qt::TextSingleLine,str).width();
    painter->setPen(m_foreground);
    painter->drawText(-w / 2, 42, str);
}

/* 绘制指针 */
void Meter::drawIndicator(QPainter *painter)
{
    painter->save();
    QPolygon pts;
    pts.setPoints(3, -2, 0, 2, 0, 0, 60);

    painter->rotate(m_startAngle);
    double degRotate = (360.0 - m_startAngle - m_endAngle) / (m_maxValue - m_minValue)*(m_value - m_minValue);

    //画指针
    painter->rotate(degRotate);                        //顺时针旋转坐标系统
    QRadialGradient haloGradient(0, 0, 60, 0, 0);      //辐射渐变
    haloGradient.setColorAt(0, QColor(60, 60, 60));
    haloGradient.setColorAt(1, QColor(160, 160, 160)); //灰
    painter->setPen(Qt::white);                        //定义线条文本颜色, 设置线条的颜色
    painter->setBrush(haloGradient);                   //刷子定义形状如何填满 填充后的颜色
    painter->drawConvexPolygon(pts);                   //这是个重载函数, 绘制多边形
    painter->restore();

    //画中心点
    QColor niceBlue(150, 150, 200);
    QConicalGradient coneGradient(0, 0, -90.0);        //角度渐变
    coneGradient.setColorAt(0.0, Qt::darkGray);
    coneGradient.setColorAt(0.2, niceBlue);
    coneGradient.setColorAt(0.5, Qt::white);
    coneGradient.setColorAt(1.0, Qt::darkGray);
    painter->setPen(Qt::NoPen);                        //没有线, 填满没有边界
    painter->setBrush(coneGradient);
    painter->drawEllipse(-5, -5, 10, 10);
}
