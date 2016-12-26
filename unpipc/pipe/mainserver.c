/*
 *更新时间: < 修改人[liyunteng] 2014/10/26 12:00:04 >
 */

#include "../unpipc.h"
#include "./comm.h"

#define SERV_FIFO "/tmp/fifo.serv"

int main(int argc, char *argv[])
{
    int readfifo, writefifo, dummyfd, fd;
    char *ptr, buf[MAXLINE + 1], fifoname[MAXLINE];
    pid_t pid;
    ssize_t n;

    if ((mkfifo(SERV_FIFO, 644)) < 0 && (errno != EEXIST))
	err_sys("can't create %s", SERV_FIFO);
    if ((readfifo = open(SERV_FIFO, O_RDONLY)) < 0)
	err_sys("open %s to read failed", SERV_FIFO);
    if ((dummyfd = open(SERV_FIFO, O_WRONLY)) < 0)
	err_sys("open %s to write failed", SERV_FIFO);

    while ((n = read(readfifo, buf, MAXLINE)) > 0) {
	if (buf[n - 1] == '\n')
	    n--;
	buf[n] = '\0';

	if ((ptr = strchr(buf, ' ')) == NULL) {
	    err_msg("bogus reques: %s", buf);
	    continue;
	}

	*ptr++ = 0;
	pid = atol(buf);
	snprintf(fifoname, sizeof(fifoname), "/tmp/fifo.%ld", (long) pid);
	if ((writefifo = open(fifoname, O_WRONLY)) < 0) {
	    err_msg("can't open: %s", fifoname);
	    continue;
	}

	printf("filename: %s", fifoname);

	if ((fd = open(ptr, O_RDONLY)) < 0) {
	    snprintf(buf + n, sizeof(buf) - n, ": can't open, %s",
		     strerror(errno));
	    n = strlen(ptr);
	    if (write(writefifo, ptr, n) < 0)
		err_msg("write failed");
	    close(writefifo);
	} else {
	    while ((n = read(fd, buf, MAXLINE)) > 0) {
		if (write(writefifo, buf, n) != n) {
		    err_msg("write failed");
		}
		printf("write %s", buf);
	    }
	    close(fd);
	    close(writefifo);
	}

    }
    return 0;
}
