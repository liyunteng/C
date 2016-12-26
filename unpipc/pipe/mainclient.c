/*
 *更新时间: < 修改人[liyunteng] 2014/10/26 12:12:59 >
 */

#include "../unpipc.h"
#include "./comm.h"

#define SERV_FIFO "/tmp/fifo.serv"

int main(int argc, char *argv[])
{
	int readfifo, writefifo;
	size_t len;
	ssize_t n;
	char *ptr, fifoname[MAXLINE], buf[MAXLINE];
	pid_t pid;

	pid = getpid();
	snprintf(fifoname, sizeof(fifoname), "/tmp/fifo.%ld", (long)pid);
	if ((mkfifo(fifoname, 644) < 0) && (errno != EEXIST)) {
		err_sys("can't create %s", fifoname);
	}
	snprintf(buf, sizeof(buf), "%ld ", (long)pid);
	len = strlen(buf);
	ptr = buf + len;

	if (fgets(ptr, MAXLINE-len, stdin) < 0) {
		err_sys("get file name failed");
	}
	len = strlen(buf);
	if ((writefifo = open(SERV_FIFO, O_WRONLY)) < 0)   {
		err_sys("can't open %s", SERV_FIFO);
	}
	if (write(writefifo, buf, len) != len) {
		err_sys("write failed");
	}
	if ((readfifo = open(fifoname, O_RDONLY)) < 0) {
		err_sys("can't open %s", fifoname);
	}
	while((n = read(readfifo, buf, MAXLINE)) > 0) {
		if (write(STDOUT_FILENO, buf, n) != n) {
			err_msg("write failed");
		}
	}
	close(readfifo);
	unlink(fifoname);
	return 0;
}
