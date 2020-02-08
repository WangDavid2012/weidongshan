/*********************************************************************************************
#####         �Ϻ�Ƕ��ʽ��԰-�������̳�         #####
#####                    www.embedclub.com                        #####
#####             http://embedclub.taobao.com               #####

* File��	globalmem_test.c
* Author:	Hanson
* Desc��	Beep test code
* History:	May 16th 2011
*********************************************************************************************/
#include <stdio.h>
#include <string.h>

int main()
{
	FILE *fp0 = NULL;
	char Buf[4096];
	int result;
	
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
	result = fwrite(Buf, sizeof(Buf), 1, fp0);
	if (result  == -1)
	{
		perror("fwrite Error!\n");
		return -1;
	}
	
	/*���¶�λ�ļ�λ�ã�˼��û�и�ָ����кκ��)*/
	fseek(fp0,0,SEEK_SET);
	
	/*���Buf*/
	strcpy(Buf,"Buf is NULL!");
	printf("BUF: %s\n",Buf);
	sleep(1);
	
	/*�����豸*/
	result = fread(Buf, sizeof(Buf), 1, fp0);
	if (result  == -1)
	{
		perror("fread Error!\n");
		return -1;
	}
	
	/*�����*/
	printf("BUF: %s\n",Buf);
	
	return 0;	

}