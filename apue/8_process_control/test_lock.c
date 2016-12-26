#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define FILE "/run/lock/lyt.lock"

int main(int argc, char *argv[])
{
    int fd;
    char buf[8];
    struct flock fl, fl_old;
    pid_t pid;

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
    errno = 0;

    if (fcntl(fd, F_SETLK, &fl) == -1) {
	fprintf(stderr, "father fcntl set lock failed: %s\n",
		strerror(errno));
	return (-1);
    }

    if ((pid = fork()) < 0) {
	fprintf(stderr, "fork failed: %s\n", strerror(errno));
	return -1;
    } else if (pid == 0) {
	fl_old.l_type = F_WRLCK;
	fl_old.l_whence = SEEK_SET;
	fl_old.l_start = 0;
	fl_old.l_len = 1;
	fl_old.l_pid = getpid();

	/* 同一个进程之内，对一个文件重复上锁不会出错,子进程不
	 * 共享父进程的文件锁 */
	if (fcntl(fd, F_SETLK, &fl_old) == -1) {
	    /* 会报错 */
	    fprintf(stderr, "child fcntl get lock  failed: %s\n",
		    strerror(errno));
	}

	sleep(5);

	if (fcntl(fd, F_SETLK, &fl_old) == -1) {
	    fprintf(stderr, "child fcntl get lock again failed: %s\n",
		    strerror(errno));
	}

    } else {
	sleep(3);

	fl.l_type = F_UNLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 1;
	fl.l_pid = getpid();

	if (fcntl(fd, F_SETLK, &fl) == -1) {
	    fprintf(stderr, "father fcntl set unlock failed: %s\n",
		    strerror(errno));
	    return (-1);
	}
	waitpid(pid, NULL, 0);
	unlink(FILE);
    }


    return 0;
}
