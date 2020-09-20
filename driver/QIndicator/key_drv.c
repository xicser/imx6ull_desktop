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
#include <linux/atomic.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/ide.h>
#include <linux/poll.h>



/* 定时器回调函数 */
static void soft_timer_callback(unsigned long arg);

/* 按键中断处理函数 */
static irqreturn_t key_irq_handler(int irq, void *dev_id);

/* key设备结构体 */
typedef struct {
    struct cdev cdev;                  //cdev
    struct class *class;               //设备类
    struct device *device;             //设备
    s32 gpio_nr;                       //gpio句柄
    struct device_node *key_nd;        //设备树节点
    dev_t devid;                       //设备号
    u16 major;                         //主设备号
    u32 minor;                         //次设备号
    s32 key_irq_nr;                    //key中断编号
    atomic_t is_valid;                 //key是否有效
} key_dev_t;

/* 定义key设备 */
static key_dev_t key_dev;

/* 软件定时器(消抖) */
static struct timer_list soft_timer;
static u32 timer_period = 10;

/* 等待队列头 */
static wait_queue_head_t r_wq_head;

/* 自旋锁 */
static spinlock_t spin_lock_key;


/* 驱动read函数 */
static ssize_t key_read(struct file *filp, char __user *buf, size_t cnt, loff_t *offt)
{
    unsigned long irq_flag;
    s32 ret;
    u8 key_value;

    if (atomic_read(&key_dev.is_valid) == 0) {
        return -EAGAIN;
    }
    spin_lock_irqsave(&spin_lock_key, irq_flag);
    key_value = gpio_get_value(key_dev.gpio_nr);
    spin_unlock_irqrestore(&spin_lock_key, irq_flag);
    atomic_set(&key_dev.is_valid, 0);
    key_value = !key_value;
    ret = copy_to_user(buf, &key_value, sizeof(key_value));

    return ret;
}
/* 驱动poll函数 */
static unsigned int key_poll(struct file *filp, struct poll_table_struct * wait)
{
    u32 mask = 0;

    poll_wait(filp, &r_wq_head, wait);

    //是否可读(设备是否就绪)
	if (atomic_read(&key_dev.is_valid)) {
		mask = POLLIN | POLLRDNORM;
	}

    return mask;
}

/* file_operations */
static const struct file_operations key_fops = {
   	.owner = THIS_MODULE,
    .read = key_read,
    .poll = key_poll
};


/* 装载 */
static int __init key_enter(void)
{
    s32 ret;

    printk("key enter\r\n");

    //初始化按键是否有效的原子变量
    atomic_set(&key_dev.is_valid, 0);

    //初始化等待队列头
    init_waitqueue_head(&r_wq_head);

    //寻找节点
    key_dev.key_nd = of_find_node_by_path("/key");
    if (key_dev.key_nd == NULL) {
        ret = -EINVAL;
        printk("find node failed\r\n");
        goto fail_find_nd;
    }

    //获取gpio句柄
    ret = of_get_named_gpio(key_dev.key_nd, "key-gpios", 0);
    if (ret < 0) {
        printk("failed to get named key-gpios\r\n");
        goto fail_named_key_gpio;
    } else {
        key_dev.gpio_nr = ret;
        printk("key gpio nr = %d\r\n", key_dev.gpio_nr);
    }

    //申请gpio(检查该gpio是否冲突)
    ret = gpio_request(key_dev.gpio_nr, "key");
    if (ret) {
        printk("key pin request failed\r\n");
        goto fail_key_pin_request;
    }

    //设置gpio
    ret = gpio_direction_input(key_dev.gpio_nr);
    if (ret) {
        printk("key pin set failed\r\n");
        goto fail_key_pin_set;
    }

    //申请设备号
    if (key_dev.major) {
        key_dev.devid = MKDEV(key_dev.major, 0);
        //"alpha_key"会出现在/proc/devices中
        ret = register_chrdev_region(key_dev.devid, 1, "alpha_key");
    } else {
        ret = alloc_chrdev_region(&key_dev.devid, 0, 1, "alpha_key");
        key_dev.major = MAJOR(key_dev.devid);
        key_dev.minor = MINOR(key_dev.devid);
    }
    if (ret < 0) {
        printk("key devid error\r\n");
        goto fail_devid;
    }

    printk("key major = %d, minor = %d\r\n", key_dev.major, key_dev.minor);

    //注册字符设备
    key_dev.cdev.owner = THIS_MODULE;
    cdev_init(&key_dev.cdev, &key_fops);
    ret = cdev_add(&key_dev.cdev, key_dev.devid, 1);
    if (ret < 0) {
        printk("cdev add failed\r\n");
        goto fail_cdev_add;
    }

    //mdev自动创建设备节点
    key_dev.class = class_create(THIS_MODULE, "key");  // 在/sys/class会出现key目录
    if (IS_ERR(key_dev.class)) {
        ret = PTR_ERR(key_dev.class);
        printk("class created failed\r\n");
		goto fail_class;
    }

    //在类下面创建设备
    // 在/sys/class/key目录下会出现akey目录
    // 在/dev目录下会出现akey设备节点
    key_dev.device = device_create(key_dev.class, NULL, key_dev.devid, NULL, "akey");
    if (IS_ERR(key_dev.device)) {
        printk("device created failed\r\n");
        goto fail_device;
    }

    //初始化软件定时器(消抖)
    init_timer(&soft_timer);
    soft_timer.function = soft_timer_callback;
    soft_timer.expires = jiffies + msecs_to_jiffies(timer_period);
    soft_timer.data = (unsigned long)&key_dev;

    //配置key中断
    key_dev.key_irq_nr = gpio_to_irq(key_dev.gpio_nr);
    // key_dev.gpio_irq_nr = irq_of_parse_and_map(key_dev.key_nd, 0);
    ret = request_irq(key_dev.key_irq_nr, key_irq_handler,
        IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "key_irq", &key_dev);
    if (ret) {
        printk("request irq failed\r\n");
        goto fail_request_irq;
    }

    return 0;

fail_request_irq:
fail_device:
    class_destroy(key_dev.class);
fail_class:
    cdev_del(&key_dev.cdev);
fail_cdev_add:
    unregister_chrdev_region(key_dev.devid, 1);    
fail_devid:
fail_key_pin_set:
    gpio_free(key_dev.gpio_nr);
fail_key_pin_request:
fail_named_key_gpio:
fail_find_nd:

    return ret;
}

/* 卸载 */
static void __exit key_exit(void)
{
    printk("key exit\r\n");

    //删除软件定时器
    del_timer_sync(&soft_timer);

    //释放中断
    free_irq(key_dev.key_irq_nr, &key_dev);

    //释放gpio
    gpio_free(key_dev.gpio_nr);

    //删除字符设备
    cdev_del(&key_dev.cdev);

    //注销设备号
    unregister_chrdev_region(key_dev.devid, 1);

    //摧毁设备
    device_destroy(key_dev.class, key_dev.devid);

    //摧毁类
    class_destroy(key_dev.class);
}

/* 按键中断处理函数 */
static irqreturn_t key_irq_handler(int irq, void *dev_id)
{
    mod_timer(&soft_timer, jiffies + msecs_to_jiffies(timer_period));
    return IRQ_HANDLED;
}

/* 定时器回调函数 */
static void soft_timer_callback(unsigned long arg)
{
    atomic_set(&key_dev.is_valid, 1);
    //唤醒进程
    wake_up(&r_wq_head);
}


module_init(key_enter);
module_exit(key_exit);

MODULE_AUTHOR("Wang Xi");
MODULE_VERSION("0.1.0");
MODULE_DESCRIPTION("key");
MODULE_LICENSE("GPL");

