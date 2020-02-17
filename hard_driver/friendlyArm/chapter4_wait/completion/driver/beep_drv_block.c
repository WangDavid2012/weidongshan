/*********************************************************************************************
#####         上海嵌入式家园-开发板商城         #####
#####                    www.embedclub.com                        #####
#####             http://embedclub.taobao.com               #####

* File：	beep_drv_block.c
* Author:	Hanson
* Desc：	Simple char device driver, Block I/O
* History:	May 16th 2011
* learn

休眠
所谓休眠就是让出CPU 然后并不返回
wait_event_interruptible(wq, condition)
condition = 0  ///休眠
condition = 1  ///唤醒

wait_event_interruptible(wq, condition)
用wake_up_interruptible()唤醒后，wait_event_interruptible(wq, condition)宏，
自身再检查“condition”这个条件以决定是返回还是继续休眠，真则返回，假则继续睡眠，
不过这个程序中若有中断程序的话，中断来了，还是会继续执行中断函数的。
只有当执行wake_up_interruptible()并且condition条件成立时才会把程序从队列中唤醒。
————————————————
版权声明：本文为CSDN博主「木木总裁」的原创文章，遵循 CC 4.0 BY-SA 版权协议，转载请附上原文出处链接及本声明。
原文链接：https://blog.csdn.net/ll148305879/article/details/92834622


*********************************************************************************************/

#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/init.h>
#include <linux/serio.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/miscdevice.h>
#include <linux/gpio.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <mach/regs-clock.h>
#include <plat/regs-timer.h>
#include <linux/wait.h>
#include <linux/sched.h>

#include <mach/regs-gpio.h>
#include <linux/cdev.h>

static int beep_major = 0;
module_param(beep_major, int, 0);
MODULE_AUTHOR("Hanson He");
MODULE_LICENSE("Dual BSD/GPL");

#define BEEP_MAGIC 'k'
#define BEEP_START_CMD _IO (BEEP_MAGIC, 1)
#define BEEP_STOP_CMD _IO (BEEP_MAGIC, 2)

void beep_start();
void beep_stop();

static DECLARE_WAIT_QUEUE_HEAD(wq);
static int flag = 0;

/*
 * Open the device; in fact, there's nothing to do here.
 */
int beep_open (struct inode *inode, struct file *filp)
{
    return 0;
}

ssize_t beep_read(struct file *file, char __user *buff, size_t count, loff_t *offp)
{
    if (file->f_flags & O_NONBLOCK) {
        printk("NONBLOCK Mode\n");
        if(flag != 0) {
            beep_stop();
        }
        return -EAGAIN;
    } else { //default use block mode
        beep_start();
        wait_event_interruptible(wq, flag != 0);
        flag = 0;
        //do sth after sleeping
        beep_stop();
    }

    return 0;
}

ssize_t beep_write(struct file *file, const char __user *buff, size_t count, loff_t *offp)
{
    // sleeping wakeup
    flag = 1;
    wake_up_interruptible(&wq);
    return 0;
}

void beep_stop( void )
{
    //add your src HERE!!!
    //set GPB0 as output
    s3c2410_gpio_cfgpin(S3C2410_GPB(0), S3C2410_GPIO_OUTPUT);
    s3c2410_gpio_setpin(S3C2410_GPB(0),0);

}

void beep_start( void )
{
    //add your src HERE!!!
    //set GPB0 as output
    s3c2410_gpio_pullup(S3C2410_GPB(0),1);
    s3c2410_gpio_cfgpin(S3C2410_GPB(0), S3C2410_GPIO_OUTPUT);
    s3c2410_gpio_setpin(S3C2410_GPB(0),1);

}

static int beep_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
    switch ( cmd ) {
		case BEEP_START_CMD: {
			beep_start();
			break;
		}
		case BEEP_STOP_CMD: {
			beep_stop();
			break;
		}
		default: {
			break;
		}
    }
    return 0;
}

static int beep_release(struct inode *node, struct file *file)
{
    return 0;
}

/*
 * Set up the cdev structure for a device.
 */
static void beep_setup_cdev(struct cdev *dev, int minor,
                            struct file_operations *fops)
{
    int err, devno = MKDEV(beep_major, minor);

    cdev_init(dev, fops);
    dev->owner = THIS_MODULE;
    dev->ops = fops;
    err = cdev_add (dev, devno, 1);
    /* Fail gracefully if need be */
    if (err)
        printk (KERN_NOTICE "Error %d adding beep%d", err, minor);
}


/*
 * Our various sub-devices.
 */
/* Device 0 uses remap_pfn_range */
static struct file_operations beep_remap_ops = {
    .owner   = THIS_MODULE,
    .open    = beep_open,
    .release = beep_release,
    .read    = beep_read,
    .write   = beep_write,
    .ioctl   = beep_ioctl,
};

/*
 * We export one beep device.  There's no need for us to maintain any
 * special housekeeping info, so we just deal with raw cdevs.
 */
static struct cdev BeepDevs;

/*
 * Module housekeeping.
 * beep_major默认值是0，MKDEV(beep_major, 0)生成一个设备号调用register_chrdev_region()
 * 如果在加载时，insmode beep.ko beep_major=10,则会调用alloc_chrdev_region
 */
static int beep_init(void)
{
    int result;
    dev_t dev = MKDEV(beep_major, 0);  //根据主设备号和从设备号生成了设备号
    char dev_name[]="beep";

    /* Figure out our device number. */
    if (beep_major)
        result = register_chrdev_region(dev, 1, dev_name);
    else {
        result = alloc_chrdev_region(&dev, 0, 1, dev_name);
        beep_major = MAJOR(dev);
    }
    if (result < 0) {
        printk(KERN_WARNING "beep: unable to get major %d\n", beep_major);
        return result;
    }
    if (beep_major == 0)
        beep_major = result;

    /* Now set up cdev. */
    beep_setup_cdev(&BeepDevs, 0, &beep_remap_ops);
    printk("beep device installed, with major %d\n", beep_major);
    printk("The device name is: %s\n", dev_name);
    return 0;
}

static void beep_cleanup(void)
{
    cdev_del(&BeepDevs);
    unregister_chrdev_region(MKDEV(beep_major, 0), 1);
    printk("beep device uninstalled\n");
}

module_init(beep_init);
module_exit(beep_cleanup);
EXPORT_SYMBOL(beep_major);
