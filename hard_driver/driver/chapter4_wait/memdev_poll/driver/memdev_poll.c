/*********************************************************************************************
#####         �Ϻ�Ƕ��ʽ��԰-�������̳�         #####
#####                    www.embedclub.com                        			#####
#####             http://embedclub.taobao.com               			#####

* File��		memdev_poll.c
* Author:		Hanson
* Desc��	Simple char device driver, non Block I/O
* History:	May 16th 2011
*********************************************************************************************/

#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/poll.h>

#include "memdev.h"

static mem_major = MEMDEV_MAJOR;

module_param(mem_major, int, S_IRUGO);

struct mem_dev *mem_devp; /*�豸�ṹ��ָ��*/

struct cdev cdev; 

bool have_data = false; // �ж��豸�Ƿ������ݿɶ�ȡ��־

/*�ļ��򿪺���*/
int mem_open(struct inode *inode, struct file *filp)
{
    struct mem_dev *dev;
    
    /*��ȡ���豸��*/
    int num = MINOR(inode->i_rdev);

    if (num >= MEMDEV_NR_DEVS) 
            return -ENODEV;
    dev = &mem_devp[num];
    
    /*���豸�����ṹָ�븳ֵ���ļ�˽������ָ��*/
    filp->private_data = dev;
    
    return 0; 
}

/*�ļ��ͷź���*/
int mem_release(struct inode *inode, struct file *filp)
{
  return 0;
}

/*������*/
static ssize_t mem_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
  unsigned long p =  *ppos;
  unsigned int count = size;
  int ret = 0;
  struct mem_dev *dev = filp->private_data; /*����豸�ṹ��ָ��*/

  printk("IN mem_read()\n");

  /*�ж϶�λ���Ƿ���Ч*/
  if (p >= MEMDEV_SIZE)
    return 0;
  if (count > MEMDEV_SIZE - p)
    count = MEMDEV_SIZE - p;

  if(down_interruptible(&dev->sem))
  	return -ERESTARTSYS;

  while (!have_data) /*û�����ݿɶ�������Ϊʲô����if������while*/
  {
	up(&dev->sem);
	if (filp->f_flags & O_NONBLOCK)
		return -EAGAIN;
	wait_event_interruptible(dev->read_queue, have_data);

	if(down_interruptible(&dev->sem))
  		return -ERESTARTSYS;
  }

  /*�����ݵ��û��ռ�*/
  if (copy_to_user(buf, (void*)(dev->data + p), count))
  {
    	ret =  - EFAULT;
	up(&dev->sem);
  } else {
    *ppos += count;
    ret = count;
    
    printk(KERN_INFO "read %d bytes(s) from %d\n", count, p);
  }

  have_data = false; /*�������������ݿɶ�*/
  up(&dev->sem);
  /*����д����*/
  wake_up(&(dev->write_queue));
 
  return ret;
}

/*д����*/
static ssize_t mem_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
  unsigned long p =  *ppos;
  unsigned int count = size;
  int ret = 0;
  struct mem_dev *dev = filp->private_data; /*����豸�ṹ��ָ��*/

  printk("IN mem_write()\n");
  
  /*�����ͻ�ȡ��Ч��д����*/
  if (p >= MEMDEV_SIZE)
    return 0;
  if (count > MEMDEV_SIZE - p)
    count = MEMDEV_SIZE - p;

  if(down_interruptible(&dev->sem))
  	return -ERESTARTSYS;

  printk("IN mem_write() - 1\n");

   while (have_data) /*���������*/
  {
	up(&dev->sem);
	if (filp->f_flags & O_NONBLOCK)
		return -EAGAIN;
	
	wait_event_interruptible(dev->write_queue, !have_data);

	if(down_interruptible(&dev->sem))
  		return -ERESTARTSYS;
  }
   printk("IN mem_write() - 2\n");
    
  /*���û��ռ�д������*/
  if (copy_from_user(dev->data + p, buf, count)) {
  	up(&dev->sem);
	ret =  - EFAULT;
  } else {
    *ppos += count;
    ret = count;
    
    printk("written %d bytes(s) from %d\n", count, p);
  }

  have_data = true; /*���µ����ݿɶ�*/
  up(&dev->sem);
  /*���Ѷ�����*/
  wake_up(&(dev->read_queue));
  printk("out of mem_write()\n");

  return ret;
}

/* seek�ļ���λ���� */
static loff_t mem_llseek(struct file *filp, loff_t offset, int whence)
{ 
    loff_t newpos;

    switch(whence) {
      case 0: /* SEEK_SET */
        newpos = offset;
        break;

      case 1: /* SEEK_CUR */
        newpos = filp->f_pos + offset;
        break;

      case 2: /* SEEK_END */
        newpos = MEMDEV_SIZE -1 + offset;
        break;

      default: /* can't happen */
        return -EINVAL;
    }
    if ((newpos<0) || (newpos>MEMDEV_SIZE))
    	return -EINVAL;
    	
    filp->f_pos = newpos;
    return newpos;

}

static unsigned int mem_poll(struct file *filp, poll_table *wait)
{
	unsigned int mask = 0;
	struct mem_dev *dev = filp->private_data; /*����豸�ṹ��ָ��*/
  
	down(&dev->sem);
  
	poll_wait(filp, &dev->read_queue, wait);
	poll_wait(filp, &dev->write_queue, wait);
	
	/*memdev�ǿ�*/
	if (have_data)
  	{
    		mask |= POLLIN | POLLRDNORM; /*��ʾ���ݿɻ��*/
		printk("In mem_poll, have_data is true\n");
 	 }
  	/*memdev����*/
  	if (!have_data)
	{
		mask |= POLLOUT | POLLWRNORM; /*��ʾ���ݿ�д��*/
		printk("In mem_poll, have_data is false\n");
	}
     
	up(&dev->sem);
	return mask;
}

/*�ļ������ṹ��*/
static const struct file_operations mem_fops =
{
  .owner = THIS_MODULE,
  .llseek = mem_llseek,
  .poll = mem_poll,
  .read = mem_read,
  .write = mem_write,
  .open = mem_open,
  .release = mem_release,
};

/*�豸����ģ����غ���*/
static int memdev_init(void)
{
  int result;
  int i;
  char dev_name[] = "memdev";

  dev_t devno = MKDEV(mem_major, 0);

  /* ��̬�����豸��*/
  if (mem_major)
    result = register_chrdev_region(devno, 2, dev_name);
  else  /* ��̬�����豸�� */
  {
    result = alloc_chrdev_region(&devno, 0, 2, dev_name);
    mem_major = MAJOR(devno);
  }  
  
  if (result < 0)
    return result;

  /*��ʼ��cdev�ṹ*/
  cdev_init(&cdev, &mem_fops);
  cdev.owner = THIS_MODULE;
  cdev.ops = &mem_fops;
  
  /* ע���ַ��豸 */
  cdev_add(&cdev, MKDEV(mem_major, 0), MEMDEV_NR_DEVS);
   
  /* Ϊ�豸�����ṹ�����ڴ�*/
  mem_devp = kmalloc(MEMDEV_NR_DEVS * sizeof(struct mem_dev), GFP_KERNEL);
  if (!mem_devp)    /*����ʧ��*/
  {
    result =  - ENOMEM;
    goto fail_malloc;
  }
  memset(mem_devp, 0, sizeof(struct mem_dev));
  
  /*Ϊ�豸�����ڴ�*/
  for (i=0; i < MEMDEV_NR_DEVS; i++) 
  {
        mem_devp[i].size = MEMDEV_SIZE;
        mem_devp[i].data = kmalloc(MEMDEV_SIZE, GFP_KERNEL);
        memset(mem_devp[i].data, 0, MEMDEV_SIZE);
	init_waitqueue_head(&mem_devp[i].read_queue);
	init_waitqueue_head(&mem_devp[i].write_queue);
	init_MUTEX(&mem_devp[i].sem);
	printk("The device name is /dev/%s%d\n", dev_name, i);
  }

  printk("mem device installed, with major %d\n", mem_major);
    
  return 0;

  fail_malloc: 
  unregister_chrdev_region(devno, 1);
  
  return result;
}

/*ģ��ж�غ���*/
static void memdev_exit(void)
{
  cdev_del(&cdev);   /*ע���豸*/
  kfree(mem_devp);     /*�ͷ��豸�ṹ���ڴ�*/
  unregister_chrdev_region(MKDEV(mem_major, 0), 2); /*�ͷ��豸��*/
}

MODULE_AUTHOR("Hanson He");
MODULE_LICENSE("GPL");

module_init(memdev_init);
module_exit(memdev_exit);
