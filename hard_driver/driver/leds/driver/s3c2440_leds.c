/*********************************************************************************************
#####         �Ϻ�Ƕ��ʽ��԰-�������̳�         #####
#####                    www.embedclub.com                        #####
#####             http://embedclub.taobao.com               #####

* File��	s3c2440_leds.c
* Author:	Hanson
* Desc��	Leds driver code
* History:	May 16th 2011
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
	 
#include <mach/regs-gpio.h>
// /home/student/linux-2.6.32.2/arch/arm/mach-s3c2410/include/mach/regs-gpio.h
#include <linux/cdev.h>

static int led_major = 0;     /* ���豸�� */
static struct cdev LedDevs;

/* Ӧ�ó���ִ��ioctl(fd, cmd, arg)ʱ�ĵ�2������ */
#define LED_MAGIC 'k'
#define IOCTL_LED_ON _IOW (LED_MAGIC, 1, int)
#define IOCTL_LED_OFF _IOW (LED_MAGIC, 2, int)
#define IOCTL_LED_RUN _IOW (LED_MAGIC, 3, int)
#define IOCTL_LED_SHINE _IOW (LED_MAGIC, 4, int)
#define IOCTL_LED_ALLON _IOW (LED_MAGIC, 5, int)
#define IOCTL_LED_ALLOFF _IOW (LED_MAGIC, 6, int)



/* ����ָ��LED���õ�GPIO���� */
static unsigned long led_table [] = {
    S3C2410_GPB(5),
    S3C2410_GPB(6),
    S3C2410_GPB(7),
    S3C2410_GPB(8),
};


/* Ӧ�ó�����豸�ļ�/dev/ledִ��open(...)ʱ��
 * �ͻ����s3c24xx_leds_open����
 */
static int s3c2440_leds_open(struct inode *inode, struct file *file)
{
    int i;    
    for (i = 0; i < 4; i++) {
        // ����GPIO���ŵĹ��ܣ���������LED���漰��GPIO������Ϊ�������
        s3c2410_gpio_cfgpin(led_table[i], S3C2410_GPIO_OUTPUT);
    }
    return 0;
}

//LEDS all light on
void leds_all_on()
{
    int i;
    for (i=0; i<4; i++) {
        s3c2410_gpio_setpin(led_table[i], 0);
    }
}

//LEDs all light off
void leds_all_off()
{
    int i;
    for (i=0; i<4; i++) {
        s3c2410_gpio_setpin(led_table[i], 1);
    }
}

/* Ӧ�ó�����豸�ļ�/dev/ledsִ��ioctl(...)ʱ��
 * �ͻ����s3c24xx_leds_ioctl����
 */
static int s3c2440_leds_ioctl(struct inode *inode, 
								struct file *file, 
								unsigned int cmd, 
								unsigned long arg)
{
    unsigned int data;
//    if (copy_from_user(&data, (unsigned int __user *)arg, sizeof(int)))
//        return -EFAULT;

// data = (unsigned int)arg;  // ����һ:���ݴ���

    if (__get_user(data, (unsigned int __user *)arg)) //������:ָ���������
        return -EFAULT;

    switch(cmd) {
        case IOCTL_LED_ON:
            // ����ָ�����ŵ������ƽΪ0
            s3c2410_gpio_setpin(led_table[data], 0);
            return 0;

        case IOCTL_LED_OFF:
            // ����ָ�����ŵ������ƽΪ1
            s3c2410_gpio_setpin(led_table[data], 1);
            return 0;
            
        case IOCTL_LED_RUN:
            // �����
            {
               int i,j;
                leds_all_off();            
                //printk("IOCTL_LED_RUN");
                for (i=0;i<data;i++)
                    for (j=0;j<4;j++) {
                        s3c2410_gpio_setpin(led_table[j], 0);
                        mdelay(400); //delay 400ms
                        s3c2410_gpio_setpin(led_table[j], 1);
                        mdelay(400); //delay 400ms
                    }  
                return 0;
             }
          
        case IOCTL_LED_SHINE:
            // LED ��˸
            {
                int i,j;
                leds_all_off();
                printk("IOCTL_LED_SHINE\n");
                for (i=0;i<data;i++) {
                    for (j=0;j<4;j++)
                        s3c2410_gpio_setpin(led_table[j], 0);
                    mdelay(400); //delay 400ms
                    for (j=0;j<4;j++)
                        s3c2410_gpio_setpin(led_table[j], 1);
                    mdelay(400);
                }
                return 0;
           }
        case IOCTL_LED_ALLON:
            // ����ָ�����ŵ������ƽΪ0
            leds_all_on();
            return 0;
        case IOCTL_LED_ALLOFF:
            // ����ָ�����ŵ������ƽΪ1
            leds_all_off();
            return 0;

        default:
            return -EINVAL;
    }
}

/* ����ṹ���ַ��豸��������ĺ���
 * ��Ӧ�ó�������豸�ļ�ʱ�����õ�open��read��write�Ⱥ�����
 * ���ջ��������ṹ��ָ���Ķ�Ӧ����
 */
static struct file_operations s3c2440_leds_fops = {
    .owner  =   THIS_MODULE,    /* ����һ���꣬�������ģ��ʱ�Զ�������__this_module���� */
    .open   =   s3c2440_leds_open,     
    .ioctl  =   s3c2440_leds_ioctl,
};


/*
 * Set up the cdev structure for a device.
 */
static void led_setup_cdev(struct cdev *dev, int minor,
		struct file_operations *fops)
{
	int err, devno = MKDEV(led_major, minor);
    
	cdev_init(dev, fops);
	dev->owner = THIS_MODULE;
	dev->ops = fops;
	err = cdev_add (dev, devno, 1);
	/* Fail gracefully if need be */
	if (err)
		printk (KERN_NOTICE "Error %d adding Led%d", err, minor);
}

/*
 * ִ�С�insmod s3c24xx_leds.ko������ʱ�ͻ�����������
 */

static int __init s3c2440_leds_init(void)
{
	int result;
	dev_t dev = MKDEV(led_major, 0);
	char dev_name[]="led";  /* ����ģʽ��ִ�С�cat /proc/devices����������豸���� */

	/* Figure out our device number. */
	if (led_major)
		result = register_chrdev_region(dev, 1, dev_name);
	else {
		result = alloc_chrdev_region(&dev, 0, 1, dev_name);
		led_major = MAJOR(dev);
	}
	if (result < 0) {
		printk(KERN_WARNING "leds: unable to get major %d\n", led_major);
		return result;
	}
	if (led_major == 0)
		led_major = result;

	/* Now set up cdev. */
	led_setup_cdev(&LedDevs, 0, &s3c2440_leds_fops);
	printk("Led device installed, with major %d\n", led_major);
	printk("The device name is: %s\n", dev_name);
	return 0;
}

/*
 * ִ�С�rmmod s3c24xx_leds������ʱ�ͻ����������� 
 */
static void __exit s3c2440_leds_exit(void)
{
    /* ж���������� */
	cdev_del(&LedDevs);
	unregister_chrdev_region(MKDEV(led_major, 0), 1);
	printk("Led device uninstalled\n");
}

/* ������ָ����������ĳ�ʼ��������ж�غ��� */
module_init(s3c2440_leds_init);
module_exit(s3c2440_leds_exit);

/* �������������һЩ��Ϣ�����Ǳ���� */
MODULE_AUTHOR("http://embedclub.taobao.com");             // �������������
MODULE_DESCRIPTION("s3c2440 LED Driver");   // һЩ������Ϣ
MODULE_LICENSE("Dual BSD/GPL");                             // ��ѭ��Э��

