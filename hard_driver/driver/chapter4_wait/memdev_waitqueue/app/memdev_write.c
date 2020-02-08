/*********************************************************************************************
#####         �Ϻ�Ƕ��ʽ��԰-�������̳�         #####
#####                    www.embedclub.com                        #####
#####             http://embedclub.taobao.com               #####

* File��		memdev_write.c
* Author:		Hanson
* Desc��	test for memdev with wait queue
* History:	May 16th 2011
*********************************************************************************************/

#include <stdio.h>
#include <string.h>

int main()
{
	FILE *fp0 = NULL;
	char Buf[128];
	
	/*��ʼ��Buf*/
	strcpy(Buf,"memdev_wq test!");
	
	/*���豸�ļ�*/
	fp0 = fopen("/dev/memdev0","r+");
	if (fp0 == NULL)
	{
		perror("Open Memdev0 Error!\n");
		return -1;
	}
	
	while (1) {
		/*д���豸*/
		fwrite(Buf, sizeof(Buf), 1, fp0);
	
		/*���¶�λ�ļ�λ�ã�˼��û�и�ָ����кκ��)*/
		fseek(fp0,0,SEEK_SET);	
	
		/*�����*/
		printf("Write Buf: %s\n",Buf);
	}
	
	return 0;	

}
