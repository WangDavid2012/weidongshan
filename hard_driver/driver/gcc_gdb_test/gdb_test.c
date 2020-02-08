//嵌入式家园培训视频讲座 www.embedclub.com
//Author: Hanson
//gdb test demo programer

#include <stdio.h>
int sum(int m);

int main()
{
	int num=0;
	num = sum(50);
	return 0;
}

int sum(int m)
{
	int i, n=0;

	for (i = 1; i <= m; i++)
	{
		n += i;
	}
	printf("The sum of 1~%d is %d\n", m,n);
	return n;
}
