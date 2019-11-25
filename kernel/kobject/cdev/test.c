#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

int
main(int argc, char *argv[])
{
    int fd;
    int val = 1;

    fd = open("/dev/lyt", O_RDWR);
    if (fd < 0) {
        printf("open file faile.\n");
        return -1;
    }

    if (argc != 2) {
        printf("Usage: %s <on|off>", argv[0]);
        return 0;
    }

    if (strcmp(argv[1], "on") == 0) {
        val = 1;
    }

    if (strcmp(argv[1], "off") == 0) {
        val = 0;
    }

    write(fd, &val, 4);
    return 0;
}
