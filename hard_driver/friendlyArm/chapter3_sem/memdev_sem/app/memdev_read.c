#include <stdio.h>
#include <string.h>

int main()
{
	FILE *fp0 = NULL;
	char Buf[128];
	

	/*���豸�ļ�*/
	fp0 = fopen("/dev/memdev0","r+");
	if (fp0 == NULL)
	{
		perror("Open Memdev0 Error!\n");
		return -1;
	}
		
	/*�����豸*/
	while(1) {
		fread(Buf, sizeof(Buf), 1, fp0);
		fseek(fp0,0,SEEK_SET);	
		/*�����*/
		printf("Read BUF: %s\n",Buf);
	}
	
	return 0;	

}
