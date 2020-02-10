/*********************************************************************************************
#####         上海嵌入式家园-开发板商城         #####
#####                    www.embedclub.com                        #####
#####             http://embedclub.taobao.com               #####

* File：	leds_les.c
* Author:	Hanson
* Desc：	Leds test code
* History:	May 16th 2011
*********************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define LED_MAGIC 'k'
#define IOCTL_LED_ON _IOW(LED_MAGIC, 1, int)
#define IOCTL_LED_OFF _IOW(LED_MAGIC, 2, int)
#define IOCTL_LED_RUN _IOW(LED_MAGIC, 3, int)
#define IOCTL_LED_SHINE _IOW(LED_MAGIC, 4, int)
#define IOCTL_LED_ALLON _IOW(LED_MAGIC, 5, int)
#define IOCTL_LED_ALLOFF _IOW(LED_MAGIC, 6, int)

void usage(char *exename)
{
    printf("Usage:\n");
    printf("    %s <led_no> <on/off>\n", exename);
    printf("    led_no = 1, 2, 3 or 4\n");
}

int main(int argc, char **argv)
{
    unsigned int led_no;
    int fd = -1;
    unsigned int count=10;

    if (argc > 3 || argc == 1)
        goto err;

    fd = open("/dev/led", 0);  // 打开设备
    if (fd < 0) {
        printf("Can't open /dev/leds\n");
        return -1;
    }

    if (argc == 2) {
        if (!strcmp(argv[1], "on")) {
            ioctl(fd, IOCTL_LED_ALLON, &count);    // 点亮它
        } else if (!strcmp(argv[1], "off")) {
            ioctl(fd, IOCTL_LED_ALLOFF, &count);   // 熄灭它
        } else if (!strcmp(argv[1], "run")) {
            ioctl(fd, IOCTL_LED_RUN, &count);   //运行跑马灯
        } else if (!strcmp(argv[1], "shine")) {
            ioctl(fd, IOCTL_LED_SHINE, &count);   //闪烁
        } else {
            goto err;
        }
    }

    if (argc == 3) {
        led_no = strtoul(argv[1], NULL, 0) - 1;    // 操作哪个LED？
        if (led_no > 3)
            goto err;
        if (!strcmp(argv[2], "on")) {
            ioctl(fd, IOCTL_LED_ON, &led_no);    // 点亮
        } else if (!strcmp(argv[2], "off")) {
            ioctl(fd, IOCTL_LED_OFF, &led_no);   // 熄灭
        } else {
            goto err;
        }
    }

    close(fd);
    return 0;

err:
    if (fd > 0)
        close(fd);
    usage(argv[0]);
    return -1;
}

