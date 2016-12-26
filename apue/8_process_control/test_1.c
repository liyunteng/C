#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <errno.h>
#include <string.h>

#define FILE "/run/lock/lyt.lock"

int main(int argc, char *argv[])
{
    int fd;
    char buf[8];
    struct flock fl;

    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 1;
    fl.l_pid = getpid();


    if ((fd = open(FILE, O_CREAT | O_RDWR)) < 0) {
	fprintf(stderr, "fopen failed: %s\n", strerror(errno));
	return (-1);
    }

    snprintf(buf, sizeof(buf) - 1, "%d", getpid());
    if (write(fd, buf, strlen(buf)) != strlen(buf)) {
	fprintf(stderr, "write pid failed: %s\n", strerror(errno));
	return (-1);
    }


    if (fcntl(fd, F_SETLK, &fl) != 0) {
	fprintf(stderr, "fcntl set lock failed: %s\n", strerror(errno));
	return (-1);
    }

    sleep(10);

    fl.l_type = F_UNLCK;
    if (fcntl(fd, F_SETLK, &fl) != 0) {
	fprintf(stderr, "fcntl set unlock failed: %s\n", strerror(errno));
	return (-1);
    }

    unlink(FILE);
    return 0;
}
