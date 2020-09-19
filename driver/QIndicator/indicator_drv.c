#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/of_gpio.h>
#include <linux/gpio.h>


/* indicator枚举 */
typedef enum {
    PIN_LED = 0,
    PIN_BEEP,
    INDICATOR_CNT   //指示器设备个数
} Indicator_t;

/* GPIO操作枚举 */
typedef enum {
    PIN_ON = 0,
    PIN_OFF = 1
} Gpio_Action_t;

/* 指示器设备结构体 */
typedef struct {
    struct cdev cdev;                       //cdev
    struct class *class;                    //设备类
    struct device *device[INDICATOR_CNT];   //设备
    u32 gpio_nr[INDICATOR_CNT];             //gpio句柄
    struct device_node *indi_nd;            //设备树节点
    dev_t devid;                            //设备号
    u16 major;                              //主设备号
    u32 minor;                              //次设备号
} indicator_dev_t;

/* 定义indicator设备 */
static indicator_dev_t indicator_dev;


/* 驱动write函数 */
static ssize_t indicator_write(struct file *filp, const char __user *buf,
            size_t count, loff_t *ppos)
{
    u32 minor = MINOR(filp->f_inode->i_rdev); //获取次设备号
    s32 failed_cnt;
    u8 data_buf;

    printk("write, minor = %d\r\n", minor);
    failed_cnt = copy_from_user(&data_buf, buf, count);
    if(failed_cnt != 0) {
        printk("data copy failed!\r\n");
        return -EIO;
    }

    if (minor == 0) { //控制led
        if (data_buf) {
            gpio_set_value(indicator_dev.gpio_nr[PIN_LED], PIN_ON);
        } else {
            gpio_set_value(indicator_dev.gpio_nr[PIN_LED], PIN_OFF);
        }
    } else { //控制beep
        if (data_buf) {
            gpio_set_value(indicator_dev.gpio_nr[PIN_BEEP], PIN_ON);
        } else {
            gpio_set_value(indicator_dev.gpio_nr[PIN_BEEP], PIN_OFF);
        }
    }

    return 0;
}


/* file_operations */
static const struct file_operations indicator_fops = {
   	.owner	= THIS_MODULE,
	.write	= indicator_write,
};


/* 装载 */
static int __init indicator_enter(void)
{
    u8 i;
    s32 ret;

    //寻找节点
    indicator_dev.indi_nd = of_find_node_by_path("/indicator");
    if (indicator_dev.indi_nd == NULL) {
        ret = -EINVAL;
        printk("can not find indicator node\n");
        goto fail_find_nd;
    }

    //获取gpio句柄
    ret = of_get_named_gpio(indicator_dev.indi_nd, "led-gpios", 0);
    if (ret < 0) {
        printk("failed to get named led-gpios\r\n");
        goto fail_named_led_gpio;
    } else {
        indicator_dev.gpio_nr[PIN_LED] = ret;
        printk("led gpio nr = %u\r\n", indicator_dev.gpio_nr[PIN_LED]);
    }
    ret = of_get_named_gpio(indicator_dev.indi_nd, "beep-gpios", 0);
    if (ret < 0) {
        printk("failed to get named beep-gpios\r\n");
        goto fail_named_beep_gpio;
    } else {
        indicator_dev.gpio_nr[PIN_BEEP] = ret;
        printk("beep gpio nr = %u\r\n", indicator_dev.gpio_nr[PIN_BEEP]);
    }

    //申请gpio(检查该gpio是否冲突)
    ret = gpio_request(indicator_dev.gpio_nr[PIN_LED], "led");
    if (ret) {
        printk("led pin request failed\r\n");
        goto fail_led_pin_request;
    }
    ret =  gpio_request(indicator_dev.gpio_nr[PIN_BEEP], "beep");
    if (ret) {
        printk("beep pin request failed\r\n");
        goto fail_beep_pin_request;
    }

    //设置gpio
    ret = gpio_direction_output(indicator_dev.gpio_nr[PIN_LED], PIN_OFF);
    if (ret) {
        printk("led pin set failed\r\n");
        goto fail_led_pin_set;
    }
    ret = gpio_direction_output(indicator_dev.gpio_nr[PIN_BEEP], PIN_OFF);
    if (ret) {
        printk("beep pin set failed\r\n");
        goto fail_beep_pin_set;
    }


    //申请设备号
    if (indicator_dev.major) {
        indicator_dev.devid = MKDEV(indicator_dev.major, 0);
        //"alpha_indicator"会出现在/proc/devices中
        ret = register_chrdev_region(indicator_dev.devid, INDICATOR_CNT, "alpha_indicator");
    } else {
        ret = alloc_chrdev_region(&indicator_dev.devid, 0, INDICATOR_CNT, "alpha_indicator");
        indicator_dev.major = MAJOR(indicator_dev.devid);
        indicator_dev.minor = MINOR(indicator_dev.devid);
    }
    if (ret < 0) {
        printk("indicator devid error\r\n");
        goto fail_devid;
    }
    for (i = 0; i < INDICATOR_CNT; i++) {
        printk("indicator(%d) major = %d, minor = %d\r\n", i + 1, indicator_dev.major, indicator_dev.minor + i);
    }

    //注册字符设备
    indicator_dev.cdev.owner = THIS_MODULE;
    cdev_init(&indicator_dev.cdev, &indicator_fops);
    ret = cdev_add(&indicator_dev.cdev, indicator_dev.devid, INDICATOR_CNT);
    if (ret < 0) {
        printk("cdev add failed\r\n");
        goto fail_cdev_add;
    }

    //mdev自动创建设备节点
    indicator_dev.class = class_create(THIS_MODULE, "indicator");  // 在/sys/class会出现indicator目录
    if (IS_ERR(indicator_dev.class)) {
        ret = PTR_ERR(indicator_dev.class);
		goto fail_class;
    }

    //在类下面创建设备
    ret = 0;
    for (i = 0; i < INDICATOR_CNT; i++) {
        // 在/sys/class/indicator目录下会出现alpha_i%d目录
        // 在/dev目录下会出现alpha_i%d设备节点
        indicator_dev.device[i] = device_create(indicator_dev.class, NULL,
                         indicator_dev.devid + i, NULL, "alpha_i%d", i);
        //只要有一个失败, 直接退出
        if (IS_ERR(indicator_dev.device[i])) {
            ret++;
        }
    }

    //检查设备创建是否成功
    if (ret) {
        for (i = 0; i < INDICATOR_CNT; i++) {
            if (!IS_ERR(indicator_dev.device[i])) {
                device_destroy(indicator_dev.class, indicator_dev.devid + i);
            }
        }
        goto fail_device;
    }
    return 0;

fail_device:
    class_destroy(indicator_dev.class);
fail_class:
    cdev_del(&indicator_dev.cdev);
fail_cdev_add:
    unregister_chrdev_region(indicator_dev.devid, INDICATOR_CNT);    
fail_devid:

fail_beep_pin_set:
fail_led_pin_set:
    gpio_free(indicator_dev.gpio_nr[PIN_BEEP]);
fail_beep_pin_request:
    gpio_free(indicator_dev.gpio_nr[PIN_LED]);
fail_led_pin_request:
fail_named_beep_gpio:
fail_named_led_gpio:
fail_find_nd:

    return ret;
}

/* 卸载 */
static void __exit indicator_exit(void)
{
    u8 i;

    printk("indicator exit\r\n");

    //关闭led
    gpio_set_value(indicator_dev.gpio_nr[PIN_LED], PIN_OFF);

    //关闭beep
    gpio_set_value(indicator_dev.gpio_nr[PIN_BEEP], PIN_OFF);

    //释放gpio
    for (i = 0; i < INDICATOR_CNT; i++) {
        gpio_free(indicator_dev.gpio_nr[i]);
    }

    //删除字符设备
    cdev_del(&indicator_dev.cdev);

    //注销设备号
    unregister_chrdev_region(indicator_dev.devid, INDICATOR_CNT);

    //摧毁设备
    for (i = 0; i < INDICATOR_CNT; i++) {
        device_destroy(indicator_dev.class, indicator_dev.devid + i);
    }

    //摧毁类
    class_destroy(indicator_dev.class);
}

module_init(indicator_enter);
module_exit(indicator_exit);

MODULE_AUTHOR("Wang Xi");
MODULE_VERSION("0.1.0");
MODULE_DESCRIPTION("indicator");
MODULE_LICENSE("GPL");

