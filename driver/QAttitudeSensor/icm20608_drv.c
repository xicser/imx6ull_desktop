#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/of_gpio.h>
#include <linux/semaphore.h>
#include <linux/timer.h>
#include <linux/i2c.h>
#include <linux/spi/spi.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/input.h>
#include <linux/of_irq.h>
#include <linux/irq.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include "icm20608_drv.h"

static bool icm20608_read_regs(u8 reg_addr, u8 *buffer, u32 len);
static bool icm20608_write_regs(u8 reg_addr, u8 *buffer, u32 len);
static bool icm20608_read_reg(u8 reg_addr, u8 *reg_val);
static bool icm20608_write_reg(u8 reg_addr, u8 reg_val);
static void icm20608_csn(bool state);
static u16  icm20608_gyro_scaleget(void);
static u16  icm20608_accel_scaleget(void);


/* icm20608数据结构体 */
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
} icm20608_adc_data_t;
#pragma pack()

/* icm20608设备结构体 */
typedef struct {
    struct cdev cdev;                  //cdev
    struct class *class;               //设备类
    struct device *device;             //设备
    s32 cs_gpio_nr;                    //片选gpio号
    dev_t devid;                       //设备号
    u16 major;                         //主设备号
    u32 minor;                         //次设备号
    struct spi_device *spi_slave;      //spi从机
    icm20608_adc_data_t adc_data;      //icm20608数据
} icm20608_dev_t;

icm20608_adc_data_t *icm20608_get_data(void);

/* 定义icm20608设备 */
static icm20608_dev_t icm20608_dev;


/* fops驱动open函数 */
static int icm20608_open(struct inode *inode, struct file *filp)
{
    u8 regval = 0;

    //IMC20608初始化
    icm20608_write_reg(ICM20_PWR_MGMT_1, 0x80);		//复位, 复位后为0x40, 睡眠模式
    mdelay(50);
    icm20608_write_reg(ICM20_PWR_MGMT_1, 0x01);		//关闭睡眠, 自动选择时钟
    mdelay(50);

    icm20608_read_reg(ICM20_WHO_AM_I, &regval);
    if((regval != ICM20608G_ID) && (regval != ICM20608D_ID)) {
        printk("read ICM20_WHO_AM_I failed\n");
        return -EIO;
    } else {
        printk("ICM20_WHO_AM_I = %x\n", regval);
    }

    icm20608_write_reg(ICM20_SMPLRT_DIV, 0x00); 	//输出速率是内部采样率
	icm20608_write_reg(ICM20_GYRO_CONFIG, 0x18); 	//陀螺仪±2000dps量程
	icm20608_write_reg(ICM20_ACCEL_CONFIG, 0x18); 	//加速度计±16G量程
	icm20608_write_reg(ICM20_CONFIG, 0x04); 		//陀螺仪低通滤波BW=20Hz
	icm20608_write_reg(ICM20_ACCEL_CONFIG2, 0x04);  //加速度计低通滤波BW=21.2Hz
	icm20608_write_reg(ICM20_PWR_MGMT_2, 0x00); 	//打开加速度计和陀螺仪所有轴
	icm20608_write_reg(ICM20_LP_MODE_CFG, 0x00); 	//关闭低功耗
	icm20608_write_reg(ICM20_FIFO_EN, 0x00);		//关闭FIFO

    icm20608_read_reg(ICM20_GYRO_CONFIG, &regval);
    printk("ICM20_GYRO_CONFIG = %#x\n", regval);
    if (regval != 0x18) {
        printk("ICM20_GYRO_CONFIG != 0x18\n");
        return -EIO;
    }

    icm20608_dev.adc_data.accelscale = icm20608_accel_scaleget();
    icm20608_dev.adc_data.gyroscale = icm20608_gyro_scaleget();
    printk("gyroscale = %d\n", icm20608_dev.adc_data.gyroscale);
    printk("accelscale = %d\n", icm20608_dev.adc_data.accelscale);

    return 0;
}
/* fops驱动read函数 */
static ssize_t icm20608_read(struct file *filp, char __user *buf,
                                        size_t cnt, loff_t *off)
{
    s32 ret;
    icm20608_adc_data_t *p_icm20608_data;

    //读取数据
    p_icm20608_data = icm20608_get_data();

    ret = copy_to_user(buf, p_icm20608_data, sizeof(icm20608_adc_data_t));
    return ret;
}
/* fops驱动release函数 */
static int icm20608_release(struct inode *inode, struct file *filp)
{
    return 0;
}

/* file_operations */
static const struct file_operations icm20608_fops = {
    .owner	= THIS_MODULE,
    .open = icm20608_open,
    .read = icm20608_read,
    .release = icm20608_release,
};

/* icm20608驱动probe函数 */
static int icm20608_probe(struct spi_device *spi)
{
    s32 ret;

    printk("icm20608 probe\n");

    icm20608_dev.spi_slave = spi;

    //申请设备号
    if (icm20608_dev.major) {
        icm20608_dev.devid = MKDEV(icm20608_dev.major, 0);
        //"icm20608"会出现在/proc/devices中
        ret = register_chrdev_region(icm20608_dev.devid, 1, "icm20608");
    } else {
        ret = alloc_chrdev_region(&icm20608_dev.devid, 0, 1, "icm20608");
        icm20608_dev.major = MAJOR(icm20608_dev.devid);
        icm20608_dev.minor = MINOR(icm20608_dev.devid);
    }
    if (ret < 0) {
        printk("icm20608 devid error\n");
        goto fail_devid;
    }

    printk("icm20608 major = %d, minor = %d\n", icm20608_dev.major, icm20608_dev.minor);

    //注册字符设备
    icm20608_dev.cdev.owner = THIS_MODULE;
    cdev_init(&icm20608_dev.cdev, &icm20608_fops);
    ret = cdev_add(&icm20608_dev.cdev, icm20608_dev.devid, 1);
    if (ret < 0) {
        printk("cdev add failed\n");
        goto fail_cdev_add;
    }

    //mdev自动创建设备节点
    icm20608_dev.class = class_create(THIS_MODULE, "spi-senser");  // 在/sys/class会出现spi-senser目录
    if (IS_ERR(icm20608_dev.class)) {
        ret = PTR_ERR(icm20608_dev.class);
        printk("class created failed\n");
        goto fail_class;
    }

    //在类下面创建设备
    // 在/sys/class/senser目录下会出现icm20608目录
    // 在/dev目录下会出现icm20608设备节点
    icm20608_dev.device = device_create(icm20608_dev.class, NULL, icm20608_dev.devid, NULL, "icm20608");
    if (IS_ERR(icm20608_dev.device)) {
        printk("device created failed\n");
        goto fail_device;
    }

    //获取片选脚
    //获取gpio句柄
    ret = of_get_named_gpio(spi->dev.of_node, "chip-select", 0);
    if (ret < 0) {
        printk("failed get cs gpio\n");
        goto fail_get_cs_gpio;
    } else {
        icm20608_dev.cs_gpio_nr = ret;
        printk("cs gpio nr = %d\n", icm20608_dev.cs_gpio_nr);
    }
    //申请gpio(检查该gpio是否冲突)
    ret = gpio_request(icm20608_dev.cs_gpio_nr, "icm20608-cs");
    if (ret) {
        printk("cs pin request failed\n");
        goto fail_cs_pin_request;
    }
    //先不要选中
    gpio_direction_output(icm20608_dev.cs_gpio_nr, 1);

    //设置spi
    spi->mode = SPI_MODE_0;
    spi->bits_per_word = 8;
    ret = spi_setup(spi);
    if (ret < 0) {
        printk("failed setup spi\n");
        goto fail_spi_setup;
    }

    return 0;

fail_spi_setup:
    gpio_free(icm20608_dev.cs_gpio_nr);
fail_cs_pin_request:
fail_get_cs_gpio:
fail_device:
    class_destroy(icm20608_dev.class);
fail_class:
    cdev_del(&icm20608_dev.cdev);
fail_cdev_add:
    unregister_chrdev_region(icm20608_dev.devid, 1);    
fail_devid:

    return ret;
}
/* spi驱动remove函数 */
static int icm20608_remove(struct spi_device *spi)
{
    printk("icm20608 remove\n");

    //取消片选
    icm20608_csn(false);

    //释放gpio
    gpio_free(icm20608_dev.cs_gpio_nr);

    //删除字符设备
    cdev_del(&icm20608_dev.cdev);

    //注销设备号
    unregister_chrdev_region(icm20608_dev.devid, 1);

    //摧毁设备
    device_destroy(icm20608_dev.class, icm20608_dev.devid);

    //摧毁类
    class_destroy(icm20608_dev.class);

    return 0;
}

/* 传统匹配方式id列表(必须要有, 即便是空的) */
static const struct spi_device_id icm20608_id_table[] = {};

/* spi设备树匹配列表 */
static const struct of_device_id icm20608_of_match_table[] = {
    { .compatible = "alpha-icm20608" },
    { /* Sentinel */ }
};

/* spi驱动 */
static struct spi_driver icm20608_spi_driver = {
    .probe = icm20608_probe,
    .remove = icm20608_remove,
    .driver = {
        .owner = THIS_MODULE,
        .name = "icm20608",
        .of_match_table = icm20608_of_match_table,
    },
    .id_table = icm20608_id_table,
};

/* 装载 */
static int __init icm20608_init(void)
{
    printk("icm20608 init\n");
    return spi_register_driver(&icm20608_spi_driver);
}
/* 卸载 */
static void __exit icm20608_exit(void)
{
    printk("icm20608 exit\n");
    spi_unregister_driver(&icm20608_spi_driver);
}

module_init(icm20608_init);
module_exit(icm20608_exit);

MODULE_AUTHOR("Wang Xi");
MODULE_VERSION("0.1.0");
MODULE_DESCRIPTION("icm20608");
MODULE_LICENSE("GPL");


/* icm20608 spi接口读n字节数据 */
static bool icm20608_read_regs(u8 reg_addr, u8 *buffer, u32 len)
{
    s32 ret_w, ret_r;

    reg_addr |= 0x80; //地址的bit7置1, 表示读取数据
    icm20608_csn(true);
    ret_w = spi_write(icm20608_dev.spi_slave, &reg_addr, 1);
    ret_r = spi_read(icm20608_dev.spi_slave, buffer, len);
    icm20608_csn(false);

    return (ret_w == 0 && ret_r == 0) ? true : false;
}
/* icm20608 spi接口写n字节数据 */
static bool icm20608_write_regs(u8 reg_addr, u8 *buffer, u32 len)
{
    s32 ret_w, ret_r;

    reg_addr &= ~0x80; //地址的bit7置0, 表示写数据
    icm20608_csn(true);
    ret_w = spi_write(icm20608_dev.spi_slave, &reg_addr, 1);
    ret_r = spi_write(icm20608_dev.spi_slave, buffer, len);
    icm20608_csn(false);

    return (ret_w == 0 && ret_r == 0) ? true : false;
}
/* icm20608 spi接口读寄存器 */
static bool icm20608_read_reg(u8 reg_addr, u8 *reg_val)
{
    return icm20608_read_regs(reg_addr, reg_val, 1);
}
/* icm20608 spi接口写寄存器 */
static bool icm20608_write_reg(u8 reg_addr, u8 reg_val)
{
    return icm20608_write_regs(reg_addr, &reg_val, 1);
}
/* icm20608 spi片选设置 */
static void icm20608_csn(bool state)
{
    if (state == true) {
        gpio_set_value(icm20608_dev.cs_gpio_nr, 0);
    } else {
        gpio_set_value(icm20608_dev.cs_gpio_nr, 1);
    }
}

/*
 * @description : 获取陀螺仪的分辨率
 * @param		: 无
 * @return		: 获取到的分辨率
 */
static u16 icm20608_gyro_scaleget(void)
{
    u8 reg_val;
	u8 data;
	u16 gyroscale;

    icm20608_read_reg(ICM20_GYRO_CONFIG, &reg_val);
	data = (reg_val >> 3) & 0x3;
	switch(data) {
		case 0: 
			gyroscale = 1310;
			break;
		case 1:
			gyroscale = 655;
			break;
		case 2:
			gyroscale = 328;
			break;
		case 3:
			gyroscale = 164;
			break;
	}
	return gyroscale;
}

/*
 * @description : 获取加速度计的分辨率
 * @param		: 无
 * @return		: 获取到的分辨率
 */
static u16 icm20608_accel_scaleget(void)
{
    u8 reg_val;
	u8 data;
	u16 accelscale;

    icm20608_read_reg(ICM20_ACCEL_CONFIG, &reg_val);
	data = (reg_val >> 3) & 0x3;
	switch(data) {
		case 0: 
			accelscale = 16384;
			break;
		case 1:
			accelscale = 8192;
			break;
		case 2:
			accelscale = 4096;
			break;
		case 3:
			accelscale = 2048;
			break;
	}
	return accelscale;
}

/* 获取ICM20608内部数据 */
icm20608_adc_data_t *icm20608_get_data(void)
{
    u8 data[14];

    icm20608_read_regs(ICM20_ACCEL_XOUT_H, data, 14); 

    //adc传感器数据
    icm20608_dev.adc_data.accel_x_adc = (s16)((data[0] << 8) | data[1]);
    icm20608_dev.adc_data.accel_y_adc = (s16)((data[2] << 8) | data[3]);
    icm20608_dev.adc_data.accel_z_adc = (s16)((data[4] << 8) | data[5]);
    icm20608_dev.adc_data.temp_adc    = (s16)((data[6] << 8) | data[7]);
    icm20608_dev.adc_data.gyro_x_adc  = (s16)((data[8] << 8) | data[9]);
    icm20608_dev.adc_data.gyro_y_adc  = (s16)((data[10] << 8) | data[11]);
    icm20608_dev.adc_data.gyro_z_adc  = (s16)((data[12] << 8) | data[13]);

    return &(icm20608_dev.adc_data);
}

