/*********************************************************************************************
#####         上海嵌入式家园-开发板商城         #####
#####                    www.embedclub.com                        #####
#####             http://embedclub.taobao.com               #####

* File：	hello.c
* Author:	Hanson
* Desc：	hello module code
* History:	May 16th 2011
*********************************************************************************************/

#include <linux/init.h>
#include <linux/module.h>
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Hanson He");

static int __init hello_init(void)
{
        printk(KERN_ALERT "Hello world\n");
        return 0;
}
static void __exit hello_exit(void)
{
        printk(KERN_ALERT " Hello world exit\n");
}
module_init(hello_init);
module_exit(hello_exit);
