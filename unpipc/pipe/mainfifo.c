/*
 *更新时间: < 修改人[liyunteng] 2014/10/26 12:32:56 >
 */

#include "../unpipc.h"
#include "./comm.h"

#define MAXLINE 1024
#define FIFO1 "/tmp/fifo.1"
#define FIFO2 "/tmp/fifo.2"

int
main(int argc, char *argv[])
{

    int   readfd, writefd;
    pid_t childpid;

    if ((mkfifo(FIFO1, 644) < 0) && (errno != EEXIST)) {
        fprintf(stderr, "mkfifo failed: %s\n", strerror(errno));
        return errno;
    }
    if ((mkfifo(FIFO2, 644) < 0) && (errno != EEXIST)) {
        unlink(FIFO1);
        fprintf(stderr, "mkfifo failed: %s\n", strerror(errno));
        return errno;
    }

    if ((childpid = fork()) == 0) {
        readfd  = open(FIFO1, O_RDONLY);
        writefd = open(FIFO2, O_WRONLY);

        server(readfd, writefd);
        return 0;
    }

    writefd = open(FIFO1, O_WRONLY);
    readfd  = open(FIFO2, O_RDONLY);

    client(readfd, writefd);
    waitpid(childpid, NULL, 0);

    close(readfd);
    close(writefd);
    unlink(FIFO1);
    unlink(FIFO2);
    return 0;
}
