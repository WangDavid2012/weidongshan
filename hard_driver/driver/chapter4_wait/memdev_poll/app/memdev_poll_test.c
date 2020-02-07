/*********************************************************************************************
#####         �Ϻ�Ƕ��ʽ��԰-�������̳�         #####
#####                    www.embedclub.com                        #####
#####             http://embedclub.taobao.com               #####

* File��	memdev_test.c
* Author:	Hanson
* Desc�� A test program in userspace   
    This example is to introduce the ways to use "select"
     and driver poll
* History:	May 16th 2011
*********************************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>

main()
{
  int fd, num;
  char Buf[128]="memdev_poll test!";
  fd_set rfds,wfds;
  
  /*�Է�������ʽ��/dev/memdev0�豸�ļ�*/
  fd = open("/dev/memdev0", O_RDWR | O_NONBLOCK);
  if (fd !=  - 1)
  {
	while (1)
    	{
      		FD_ZERO(&rfds);
      		FD_ZERO(&wfds);
      		FD_SET(fd, &rfds);
      		FD_SET(fd, &wfds);

      		select(fd + 1, &rfds, &wfds, NULL, NULL);
      		/*���ݿɻ��*/
      		if (FD_ISSET(fd, &rfds))
      		{
      			printf("Poll monitor:can be read\n");
			lseek(fd, 0, SEEK_SET);
			read(fd, Buf, sizeof(Buf));
			printf("Read Buf: %s\n", Buf);
			sleep(1);
      		}
      		/*���ݿ�д��*/
      		if (FD_ISSET(fd, &wfds))
      		{
      			printf("Poll monitor:can be written\n");
			lseek(fd, 0, SEEK_SET);
			write(fd, Buf, sizeof(Buf));
			printf("Write Buf: %s\n", Buf);
			sleep(1);
      		}      
   	}
  }
  else
  {
    printf("Device open failure\n");
  }
}
