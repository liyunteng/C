/*
 *更新时间: < 修改人[liyunteng] 2014/10/26 13:40:21 >
 */

#include "../unpipc.h"
#include "./comm.h"

#define FIFO1 "/tmp/fifo.1"
#define FIFO2 "/tmp/fifo.2"

int
main(int argc, char *argv[])
{
    int readfd, writefd;

    if ((mkfifo(FIFO1, 644) < 0) && (errno != EEXIST))
        err_sys("can't create %s", FIFO1);
    if ((mkfifo(FIFO2, 644) < 0) && (errno != EEXIST)) {
        unlink(FIFO1);
        err_sys("can't create %s", FIFO2);
    }

    while (1) {
        if ((readfd = open(FIFO1, O_RDONLY)) < 0) {
            err_sys("can't open %s", FIFO1);
        }
        if ((writefd = open(FIFO2, O_WRONLY)) < 0) {
            err_sys("can't open %s", FIFO2);
        }

        server(readfd, writefd);

        close(readfd);
        close(writefd);
    }

    unlink(FIFO1);
    unlink(FIFO2);

    return 0;
}
