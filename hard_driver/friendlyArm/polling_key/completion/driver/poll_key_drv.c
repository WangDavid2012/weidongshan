/*********************************************************************************************
#####         �Ϻ�Ƕ��ʽ��԰-�������̳�         #####
#####                    www.embedclub.com                        #####
#####             http://embedclub.taobao.com               #####

* File��		poll_key_drv.c
* Author:		Hanson
* Desc��	Key scan (with polling method) device driver 
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
#include <linux/wait.h>
#include <linux/sched.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <mach/regs-clock.h>
#include <plat/regs-timer.h>	 
#include <mach/regs-gpio.h>
#include <linux/cdev.h>
#include <linux/miscdevice.h>
#include <linux/gpio.h>
#include <asm-generic/poll.h>
#include <linux/poll.h>

static int key_major = 0;
struct key_desc_t {
	int pin;    //which gpio pin
	int pin_dir;//input or output
	int number;
    	char *name;	
};

#define MAX_KEYS    6
/* ����ָ����������gpio�ı��,��������� */
static struct key_desc_t key_desc [] = {
    {S3C2410_GPG(0), S3C2410_GPIO_INPUT, 0, "KEY1"}, /* K1 */
    {S3C2410_GPG(3),  S3C2410_GPIO_INPUT,  1, "KEY2"}, /* K2 */
    {S3C2410_GPG(5),  S3C2410_GPIO_INPUT,  2, "KEY3"}, /* K3 */
    {S3C2410_GPG(6),  S3C2410_GPIO_INPUT,  3, "KEY4"}, /* K4 */
    {S3C2410_GPG(7),  S3C2410_GPIO_INPUT,  4, "KEY5"}, /* K5 */
    {S3C2410_GPG(11),  S3C2410_GPIO_INPUT, 5, "KEY6"}, /* K6 */


};

/* ���������µĴ���*/
static volatile int key_values [MAX_KEYS] = {0, 0, 0, 0, 0, 0};

/* �ȴ�����: 
 * ��û�а���������ʱ������н��̵���key_read������
 * ��������
 */
static DECLARE_WAIT_QUEUE_HEAD(key_waitq);

/*�ں˶�ʱ��*/
static struct timer_list key_timer;

#define KEY_TIMER_DELAY    HZ/50   /*20ms*/

/* �¼���־, �м�����ʱ������1��key_read������0 */
static volatile int ev_press = 0;


static void key_timer_handle(unsigned long data)
{
	struct key_desc_t *key_p = (struct key_desc_t *)data;
	int down;
	static unsigned char pressed[MAX_KEYS] = {0};
	int i;
	
	for(i=0;i<MAX_KEYS;i ++,key_p ++)
	{
    		down = s3c2410_gpio_getpin(key_p->pin);
		if(!down) //key pressed
		{
			pressed[i] ++;
			if(pressed[i] > 2) pressed[i] = 3;
		}
		if((!down) && (2 == pressed[i]))//Ҫ�ȴ�20ms�ټ��һ�¸ð���ȷʵ�Ǳ�����,�����Ƕ���
		{
			printk("pressed\n");
			key_values[i] = i + 1;
    			ev_press = 1;                  /* ��ʾ������������ */
    			wake_up_interruptible(&key_waitq);   /* �������ߵĽ��� */
		}
		if(down && pressed[i]) //����ü����ͷ�
		{
			pressed[i] = 0;
		}
	}
	key_timer.expires = jiffies + KEY_TIMER_DELAY;
	add_timer(&key_timer);

}


/* Ӧ�ó�����豸�ļ�/dev/keyִ��open(...)ʱ��
 * �ͻ����key_open����
 */
static int key_open(struct inode *inode, struct file *file)
{
    return 0;
}


/* Ӧ�ó�����豸�ļ�/dev/keyִ��close(...)ʱ��
 * �ͻ����key_close����
 */
static int key_close(struct inode *inode, struct file *file)
{
    return 0;
}


/* Ӧ�ó�����豸�ļ�/dev/keyִ��read(...)ʱ��
 * �ͻ����key_read����
 */
static int key_read(struct file *filp, char __user *buff, 
                        size_t count, loff_t *offp)
{
    unsigned long err;

	if (!ev_press) {
		if (filp->f_flags & O_NONBLOCK)
			return -EAGAIN;
		else
			/* ���ev_press����0������ */
			wait_event_interruptible(key_waitq, ev_press);
	}
    
    /* ִ�е�����ʱ��ev_press����1��������0 */
    ev_press = 0;

    /* ������״̬���Ƹ��û�������0 */
    err = copy_to_user(buff, (const void *)key_values, min(sizeof(key_values), count));
    memset((void *)key_values, 0, sizeof(key_values));

    return err ? -EFAULT : min(sizeof(key_values), count);
}

/**************************************************
* ���û��������select����ʱ��������������
* ����а������ݣ���select���������̷���
* ���û�а������ݣ�������ʹ��poll_wait�ȴ�
**************************************************/
static unsigned int key_poll(struct file *file,
        			 struct poll_table_struct *wait)
{
	unsigned int mask = 0;
    	poll_wait(file, &key_waitq, wait);
    	if (ev_press)
        	mask |= POLLIN | POLLRDNORM;
    	return mask;
}


/* ����ṹ���ַ��豸��������ĺ���
 * ��Ӧ�ó�������豸�ļ�ʱ�����õ�open��read��write�Ⱥ�����
 * ���ջ��������ṹ�еĶ�Ӧ����
 */
static struct file_operations key_fops = {
    .owner   =   THIS_MODULE,    /* ����һ���ָ꣬�����ģ��ʱ�Զ�������__this_module���� */
    .open    =   key_open,
    .release =   key_close, 
    .read    =   key_read,
    .poll    =   key_poll,
};


void set_gpio_mode_for_key(struct key_desc_t * key)
{
	int i;

	for(i = 0; i < MAX_KEYS; i ++,key ++)
	{
		s3c2410_gpio_pullup(key->pin, 1);
		s3c2410_gpio_cfgpin(key->pin, key->pin_dir);
	}

}

/*
 * Set up the cdev structure for a device.
 */
static void key_setup_cdev(struct cdev *dev, int minor,
                struct file_operations *fops)
{
        int err, devno = MKDEV(key_major, minor);

        cdev_init(dev, fops);
        dev->owner = THIS_MODULE;
        dev->ops = fops;
        err = cdev_add (dev, devno, 1);
        /* Fail gracefully if need be */
        if (err)
                printk (KERN_NOTICE "Error %d adding key%d", err, minor);
}


/*
 * We export one key device.  There's no need for us to maintain any
 * special housekeeping info, so we just deal with raw cdev.
 */
static struct cdev key_cdev;


/*
 * ִ�С�insmod key_drv.ko������ʱ�ͻ�����������
 */
static int __init button_init(void)
{
        int result;
        dev_t dev = MKDEV(key_major, 0);
	char dev_name[]="key";
                                                                                                         
        /* Figure out our device number. */
        if (key_major)
                result = register_chrdev_region(dev, 1, dev_name);
        else {
                result = alloc_chrdev_region(&dev, 0, 1, dev_name);
                key_major = MAJOR(dev);
        }
        if (result < 0) {
                printk(KERN_WARNING "key: unable to get major %d\n", key_major);
                return result;
        }
        if (key_major == 0)
                key_major = result;
                                                                                                         
        /* Now set up cdev. */
        key_setup_cdev(&key_cdev, 0, &key_fops);
        printk("key device installed, with major %d\n", key_major);
	printk("The device name is: /dev/%s\n", dev_name);

	/*set gpio mode*/
	set_gpio_mode_for_key(key_desc);
	/*init kernel timer*/
	init_timer(&key_timer);
	key_timer.function = &key_timer_handle;
	key_timer.data = (unsigned long)key_desc;
	key_timer.expires = jiffies + KEY_TIMER_DELAY;
	
	add_timer(&key_timer);
 
	return 0;
}

/*
 * ִ�Сrrmmod key_drv.ko������ʱ�ͻ����������� 
 */
static void __exit button_cleanup(void)
{
        cdev_del(&key_cdev);
        unregister_chrdev_region(MKDEV(key_major, 0), 1);
	del_timer_sync(&key_timer);
        printk("key device uninstalled\n");
}

/* ������ָ����������ĳ�ʼ��������ж�غ��� */
module_init(button_init);
module_exit(button_cleanup);

/* �������������һЩ��Ϣ�����Ǳ���� */
MODULE_AUTHOR("Hanson he");             // �������������
MODULE_DESCRIPTION("S3C2440 KEY Driver");   // һЩ������Ϣ
MODULE_LICENSE("GPL");                              // ��ѭ��Э��

