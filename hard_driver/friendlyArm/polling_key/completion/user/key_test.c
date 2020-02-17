/*********************************************************************************************
#####         �Ϻ�Ƕ��ʽ��԰-�������̳�         #####
#####                    www.embedclub.com                        #####
#####             http://embedclub.taobao.com               #####

* File��		key_test.c
* Author:		Hanson
* Desc��	a test for Key scan (with polling method) device driver 
* History:	May 16th 2011
*********************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>

int main(void)
{
	int i;
	int key_fd;
	int key_value[6];

	/*�򿪼����豸�ļ�*/
	key_fd = open("/dev/key", 0);
	if (key_fd < 0) {
		perror("open device key");
		exit(1);
	}

	for (;;) {
		fd_set rds;
		int ret;

		FD_ZERO(&rds);
		FD_SET(key_fd, &rds);

		/*ʹ��ϵͳ����select����Ƿ��ܹ���/dev/key�豸��ȡ����*/
		ret = select(key_fd + 1, &rds, NULL, NULL, NULL);
		
		/*��ȡ�������˳�����*/
		if (ret < 0) {
			perror("select");
			exit(1);
		}
		
		if (ret == 0) {
			printf("Timeout.\n");
		} 
		/*�ܹ���ȡ������*/
		else if (FD_ISSET(key_fd, &rds)) {
			/*��ʼ��ȡ�����������������ݣ�ע��key_value�ͼ��������ж���Ϊһ�µ�����*/
			int ret = read(key_fd, key_value, sizeof key_value);
			if (ret != sizeof key_value) {
				if (errno != EAGAIN)
					perror("read key\n");
				continue;
			} else {
				/*��ӡ��ֵ*/
				for (i = 0; i < 6; i++)
				    printf("K%d %s, key value = 0x%02x\n", \
					   i+1, (key_value[i] & 0x80) ? "released": \
		                           key_value[i] ? "pressed down" : "", key_value[i]);
			}
				
		}
	}
	/*�ر��豸�ļ����*/
	close(key_fd);
	return 0;
}
