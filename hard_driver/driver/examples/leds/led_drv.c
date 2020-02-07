/**********************************************************
 文件说明：david 2018/10/30 创建，主要用于学习linux驱动程序
*********************************************************/
#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <asm/irq.h>
#include <mach/regs-gpio.h>
#include <mach/hardware.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/string.h>
#include <linux/list.h>
#include <linux/pci.h>
#include <linux/gpio.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <asm/unistd.h>



/*********************************************************************************
关于头文件的说明：
	s3c2410_gpio_cfgpin() 这个函数在哪定义的？
这些和体系结构有关：
在linux-2.6.32.2/arch/arm/mach-s3c2410/include/mach/hardware.h 文件中可以找到函数的定义。
在linux-2.6.32.2/arch/arm/plat-s3c2410/gpio.c 中，可以找到函数的实现。
 	实际上，我们并不关心这些，写驱动只要会使用它们就可以了，除非你所用的CPU体系平台还没有被
 linux支持。
***************************************************************************************/

#define DEVICE_NAME "leds"         //声明这个宏有什么作用

static unsigned long led_table[] = {
	S3C2410_GPB(5);
	S3C2410_GPB(6);
	S3C2410_GPB(7);
	S3C2410_GPB(8);	

};
static unsigned ing led_cfg_table[] = {
	S3C2410_GPIO_OUTPUT,
	S3C2410_GPIO_OUTPUT,
	S3C2410_GPIO_OUTPUT,
	S3C2410_GPIO_OUTPUT,
};

/***********************************************************************************
time:2018/10/30  david 学习驱动知识中的miscdevice
函数分析：
文件中使用到的几个结构体：
	ioctl,
	inode,
	file
	miscdevice
	
	结构体之间的包含关系：
		miscdevice
			file_operations
				ioctl
				inode
				
				
struct miscdevice  {
    int minor;                       //次设备号  通常为MISC_DYNAMIC_MINOR动态分配
    const char *name;                //设备为的名字
    const struct file_operations *fops;//函数操作集
    struct list_head list;
    struct device *parent;
    struct device *this_device;
    const char *nodename;
    umode_t mode;
};
********************************************************************************/




static int s3c2440_leds_ioctl(
	struct inode *inode,
	struct file *file,
	unsigned int cmd,
	unsigned long arg)
{

	switch(cmd)
	{
		case 0:
		case 1:
			if(arg > 4)
				return -EINVAL;
			s3c2410_gpio_setpin(led_table[arg],!cmd);
			return 0;
	}
	default:
		return -EINVAL;

}
static struct file_operations dev_fops = {
	.owner = THIS_MODULE,
	.ioctl = s3c2440_leds_ioctl,

};

static struct miscdevice misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &dev_fops,
};

static int __init dev_init(void)
{
	int ret;
	int i;
	for(i=0;i<4;i++)
	{
		s3c2410_gpio_cfgpin(led_table[i],led_cfg_table);
		s3c2410_gpio_setpin(led_table[i],0);
	}
	ret = misc_register(&misc);
	printk(DEVICE_NAME"\tinitialized\n");
	
	return ret;
	
}
static void __exit dev_exit(void)
{
	misc_deregister(&misc);

}

module_init(dev_init);
module_exit(dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("David");





































