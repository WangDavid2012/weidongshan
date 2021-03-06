/*********************************************************************************************
#####         上海嵌入式家园-开发板商城         #####
#####                            www.embedclub.com                               #####
#####                     http://embedclub.taobao.com                       #####

* File：		memdev_test.c
* Author:		Hanson
* Desc：	virual memory  device driver test code
* History:	May 20th 2011
*********************************************************************************************/
#include <stdio.h>
#include <string.h>

int main()
{
	FILE *fp0 = NULL;
	char Buf[4096];
	int result;
	
	/*初始化Buf*/
	strcpy(Buf,"Mem is char dev!");
	printf("BUF: %s\n",Buf);
	
	/*打开设备文件*/
	fp0 = fopen("/dev/memdev0","r+");
	if (fp0 == NULL)
	{
		perror("Open Memdev0 Error!\n");
		return -1;
	}
	
	/*写入设备*/
	result = fwrite(Buf, sizeof(Buf), 1, fp0);
	if (result  == -1)
	{
		perror("fwrite Error!\n");
		return -1;
	}
	
	/*重新定位文件位置（思考没有该指令，会有何后果)*/
	fseek(fp0,0,SEEK_SET);
	
	/*清除Buf*/
	strcpy(Buf,"Buf is NULL!");
	printf("BUF: %s\n",Buf);
	
	sleep(1);
	/*读出设备*/
	result = fread(Buf, sizeof(Buf), 1, fp0);
	if (result  == -1)
	{
		perror("fread Error!\n");
		return -1;
	}
	
	/*检测结果*/
	printf("BUF: %s\n",Buf);
	
	return 0;	

}
