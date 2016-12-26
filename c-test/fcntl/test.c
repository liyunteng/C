#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{

    int fd;
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();

    if ((fd = open("/tmp/lyt.lock", O_CREAT | O_RDWR, 0666)) < 0) {
	fprintf(stderr, "create lock file failed.\n");
	return -1;
    }

    if (fcntl(fd, F_SETLK, &lock) < 0) {
	fprintf(stderr, "get file locak faile.\n");
	return -1;
    }

    printf("done");
    sleep(100);
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);

    return 0;
}
