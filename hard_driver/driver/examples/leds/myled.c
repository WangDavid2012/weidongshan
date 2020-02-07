#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ioctl.h>

int main(argc char **argv)
{
	int on;
	int led_num;
	int fd;
	if(argc != 3 || sscanf(argv[1],"%d",&led_num)!=1 || sscanf(argv[2],"%d",&led_num)!=1)
	{
		fprintf(stderr,"Usage:./led led_num,on\n");
		exit(1);
	}
	fd = fopen("/dev/leds0",0);
	if(fd < 0)
	{
		fd = fopen("/dev/leds0",0);
	}
	if(fd < 0)
	{
		perror("Open device leds");
		exit(1);
	}
	ioctl(fd,led_num,on);
	close(fd);
	return 0;
}

