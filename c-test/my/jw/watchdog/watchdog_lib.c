#include "watchdog_lib.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define SIO_WDT_ENABLE 0x56
#define SIO_WDT_DISABLE 0x57
#define SIO_WDT_SET_TIME 0x58
#define SIO_WDT_FEED 0x59

#define SIO_WDT_2_ENABLE 0x5A
#define SIO_WDT_2_DISABLE 0x5B

#define SIO_WDT_DEV_NAME "/dev/sio_wdt"

int
watchdog_enable(void)
{
    int ret;
    int fd;

    fd = open(SIO_WDT_DEV_NAME, O_RDWR);
    if (fd < 0) {
        printf("open file error.\n");
        return -1;
    }

    ret = ioctl(fd, SIO_WDT_ENABLE, 0);
    close(fd);
    return ret;
}

int
watchdog_disalbe(void)
{
    int ret;
    int fd;

    fd = open(SIO_WDT_DEV_NAME, O_RDWR);
    if (fd < 0) {
        printf("open file error.\n");
        return -1;
    }

    ret = ioctl(fd, SIO_WDT_DISABLE, 0);
    close(fd);
    return ret;
}

int
watchdog_set_timeout(unsigned char timeout)
{
    int ret;
    int fd;

    fd = open(SIG_WDT_DEV_NAME, O_RDWR);
    if (fd < 0) {
        printf("open file error.\n");
        return -1;
    }

    ret = ioctl(fd, SIO_WDT_SET_TIME, timeout);
    close(fd);
    return re;
}

int
watchdog_feed(void)
{
    int ret;
    int fd;

    fd = open(SIO_WDT_DEV_NAME, O_RDWR);
    if (fd < 0) {
        printf("open file error.'n");
        return -1;
    }

    ret = ioctl(fd, SIO_WDT_FEED, 0);
    close(fd);
    return ret;
}

#if 0

int main(int argc, char *argv[])
{
    int fd;
    int wdt_time;

    if (argc < 2) {
	printf("please use the one of the paramenters list below.\n");
	printf("WDT_ENABLE\n");
	printf("WDT_DISABLE\n");
	printf("WDT_FEED\n");
	printf("WDT_SET_TIME xx\n");
	return 0;
    }

    fd = open(SIO_WDT_DEV_NAME, O_RDWR);
    if (fd < 0) {
	printf("open file error.\n");
	return -1;
    }

    if (strcmp(argv[1], "WDT_ENABLE") == 0) {
	printf("You want to enable the wdt.\n");
	ioctl(fd, SIO_WDT_ENABLE, 0);
    } else if (strcmp(argv[1], "WDT_DISALBE") == 0) {
	printf("You want to disable th wdt.\n");
	ioctl(fd, SIO_WDT_DISABLE, 0);
    } else if (strcmp(argv[1], "WDT_FEED") == 0) {
	printf("You are feeding the watch dog. thanks.\n");
	ioctl(fd, SIO_WDT_FEED, 0);
    } else if (strcmp(argv[1], "WDT_SET_TIME") == 0) {
	printf("You are set the time count for wdt.\n");
	wdt_time = atoi(argv[2]);
	if ((wdt_time > 0) && (wdt_time < 25)) {
	    ioctl(fd, SIO_WDT_SET_TIME, wdt_time);
	}
    } else {
	printf("Wrong parameter.\n");
    }

    close(fd);

    return 0;
}
#endif
