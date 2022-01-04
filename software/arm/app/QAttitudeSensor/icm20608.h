#ifndef ICM20608_H
#define ICM20608_H

#include "config.h"
#include <QThread>

//icm20608 adc数据结构体
#pragma pack(1)
typedef struct {
    s16 gyro_x_adc;     //陀螺仪X轴adc值
    s16 gyro_y_adc;     //陀螺仪Y轴adc值
    s16 gyro_z_adc;     //陀螺仪Z轴adc值
    s16 accel_x_adc;    //加速度计X轴adc值
    s16 accel_y_adc;    //加速度计Y轴adc值
    s16 accel_z_adc;    //加速度计Z轴adc值
    s16 temp_adc;       //温度adc值
    u16 gyroscale;      //陀螺仪分辨率(*10)
    u16 accelscale;     //加速度计分辨率
} Icm20608_Adc_Data_t;
#pragma pack()

//icm20608实际数据结构体
typedef struct {
    float gyro_x_act;     //陀螺仪X轴实际值
    float gyro_y_act;     //陀螺仪Y轴实际值
    float gyro_z_act;     //陀螺仪Z轴实际值
    float accel_x_act;    //加速度计X轴实际值
    float accel_y_act;    //加速度计Y轴实际值
    float accel_z_act;    //加速度计Z轴实际值
    float temp_act;       //温度实际值
} Icm20608_Act_Data_t;

class Icm20608 : public QThread
{
    Q_OBJECT

public:
    explicit Icm20608(QThread *parent = 0);
    ~Icm20608();

protected:
    void run(void);

private:
    int fd;                                                //文件描述符

signals:
    void icm20608_data_signal(Icm20608_Act_Data_t *data);  //icm20608数据信号
};

#endif // ICM20608_H
