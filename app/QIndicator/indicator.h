#ifndef INDICATOR_H
#define INDICATOR_H


//指示器枚举
typedef enum {
    INDICATOR_LED,
    INDICATOR_BEEP,
    INDICATOR_CNT
} Indicator_t;

//指示器状态枚举
typedef enum {
    INDICATOR_ON,
    INDICATOR_OFF
} Status_t;


class Indicator
{
public:
    Indicator();
    static void indicatorControl(Indicator_t which, Status_t status);      //控制led或者beep
};

#endif // INDICATOR_H
