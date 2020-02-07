/********************************************************************************************
* File��	beep_test.c
* Author:	Hanson
* Desc��	Beep test code
* History:	May 16th 2011
�ļ�˵�������Ʒ���������
	ִ�г��򣬷������죬Ȼ�������ַ���������ֹͣ��
	
time:2018/12/10
author:david
*********************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/ioctl.h>

#define BEEP_MAGIC 'k'
#define BEEP_START_CMD _IO (BEEP_MAGIC, 1)
#define BEEP_STOP_CMD  _IO (BEEP_MAGIC, 2)



int main()
{
	int i = 0;
	int dev_fd;
	dev_fd = open("/dev/beep",O_RDWR | O_NONBLOCK);
	if ( dev_fd == -1 ) {
		printf("Cann't open file /dev/beep\n");
		exit(1);
	}
	printf("Start beep\n");
	ioctl (dev_fd, BEEP_START_CMD,0);
	getchar();
	ioctl (dev_fd, BEEP_STOP_CMD,0);
	printf("Stop beep and Close device\n");
	close(dev_fd);
	return 0;
}
