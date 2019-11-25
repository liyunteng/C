/*
 * test.c - test
 *
 * Date   : 2019/11/25
 */
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>

#define FILE_NAME "/dev/lyt"

int
main(void)
{
    int fd;
    fd = open(FILE_NAME, O_RDWR);
    assert(fd > 0);

    char buf[128];
    int  n = read(fd, buf, 128);
    assert(n > 0);
    buf[n] = 0;
    printf("%s\n", buf);

    const char *msg = "abc";
    n               = write(fd, msg, strlen(msg) + 1);
    assert(n > 0);

    printf("after write\n");
    n = read(fd, buf, 128);
    assert(n > 0);
    buf[n] = 0;
    printf("%s\n", buf);

    close(fd);
    return 0;
}
