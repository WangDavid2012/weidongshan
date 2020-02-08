/*********************************************************************************************
#####         �Ϻ�Ƕ��ʽ��԰-�������̳�         #####
#####                    www.embedclub.com                        #####
#####             http://embedclub.taobao.com               #####

* File��	memdev_test.c
* Author:	Hanson
* Desc��	 test  memdev_test driver
* History:	May 16th 2011
*********************************************************************************************/

#include <stdio.h>
#include <string.h>

int main()
{
	FILE *fp0 = NULL;
	char Buf[4096];
	
	/*��ʼ��Buf*/
	strcpy(Buf,"Mem is char dev!");
	printf("BUF: %s\n",Buf);
	
	/*���豸�ļ�*/
	fp0 = fopen("/dev/globalmem","r+");
	if (fp0 == NULL)
	{
		perror("Open globalmem Error!\n");
		return -1;
	}
	
	/*д���豸*/
	fwrite(Buf, sizeof(Buf), 1, fp0);
	
	/*���¶�λ�ļ�λ�ã�˼��û�и�ָ����кκ��)*/
	fseek(fp0,0,SEEK_SET);
	
	/*���Buf*/
	strcpy(Buf,"Buf is NULL!");
	printf("BUF: %s\n",Buf);
	
	
	/*�����豸*/
	fread(Buf, sizeof(Buf), 1, fp0);
	
	/*�����*/
	printf("BUF: %s\n",Buf);
	
	return 0;	

}
