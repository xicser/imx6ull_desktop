#include "icm20608.h"
#include "stdio.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "sys/ioctl.h"
#include "fcntl.h"
#include "stdlib.h"
#include "string.h"
#include <poll.h>
#include <sys/select.h>
#include <sys/time.h>
#include <signal.h>
#include <fcntl.h>

static Icm20608_Adc_Data_t icm20608_adc_data;
static Icm20608_Act_Data_t icm20608_act_data;

Icm20608::Icm20608(QThread *parent) : QThread(parent)
{

}

Icm20608::~Icm20608()
{
    close(fd);
}

void Icm20608::run(void)
{
    int ret = 0;

    fd = open("/dev/icm20608", O_RDWR);
    if(fd < 0) {
        printf("can't open file /dev/icm20608\n");
        return;
    }
    while (1) {
        ret = read(fd, &icm20608_adc_data, sizeof(Icm20608_Adc_Data_t));
        if(ret == 0) {
            //计算实际数据
            icm20608_act_data.gyro_x_act = (float)icm20608_adc_data.gyro_x_adc / (icm20608_adc_data.gyroscale / 10.0);
            icm20608_act_data.gyro_y_act = (float)icm20608_adc_data.gyro_y_adc / (icm20608_adc_data.gyroscale / 10.0);
            icm20608_act_data.gyro_z_act = (float)icm20608_adc_data.gyro_z_adc / (icm20608_adc_data.gyroscale / 10.0);

            icm20608_act_data.accel_x_act = (float)icm20608_adc_data.accel_x_adc / icm20608_adc_data.accelscale;
            icm20608_act_data.accel_y_act = (float)icm20608_adc_data.accel_y_adc / icm20608_adc_data.accelscale;
            icm20608_act_data.accel_z_act = (float)icm20608_adc_data.accel_z_adc / icm20608_adc_data.accelscale;
            icm20608_act_data.temp_act = ((float)icm20608_adc_data.temp_adc - 25 ) / 326.8 + 25;

            //通知显示主进程
            emit icm20608_data_signal(&icm20608_act_data);
        }
        usleep(1000 * 1 * 20);
    }
}
