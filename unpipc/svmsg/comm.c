/*
 *更新时间: < 修改人[liyunteng] 2014/10/26 13:58:39 >
 */

#include "./comm.h"
#include "../unpipc.h"
#include "svmsg.h"
#include <sys/msg.h>

ssize_t
msg_send(int fd, struct msg *mptr)
{
    return (msgsnd(fd, &mptr->msg_type, mptr->msg_len, 0));
}

ssize_t
msg_recv(int fd, struct msg *mptr)
{
    ssize_t n;

    n             = msgrcv(fd, &mptr->msg_type, MAXMSG, mptr->msg_type, 0);
    mptr->msg_len = n;
    return (n);
}

void
client(int readfd, int writefd)
{
    size_t     len;
    ssize_t    n;
    struct msg msg;

    if (fgets(msg.msg_data, MSGDATA, stdin) < 0)
        err_sys("get file name failed");

    len = strlen(msg.msg_data);
    if (msg.msg_data[len - 1] == '\n')
        len--;
    msg.msg_len  = len;
    msg.msg_type = 1;

    if (msg_send(writefd, &msg) < 0) {
        err_sys("message send failed");
    }

    while ((n = msg_recv(readfd, &msg)) > 0) {
        if (write(STDOUT_FILENO, msg.msg_data, n) != n) {
            err_sys("write data failed");
        }
    }
}

void
server(int readfd, int writefd)
{
    int        fd;
    ssize_t    n;
    size_t     times;
    struct msg msg;

    if ((n = msg_recv(readfd, &msg)) == 0) {
        return;
        err_ret("pathname missing");
    }

    msg.msg_data[n] = '\0';

    if ((fd = open(msg.msg_data, O_RDONLY)) < 0) {
        snprintf(msg.msg_data + n, sizeof(msg.msg_data) - n, ":can't open, %s\n", strerror(errno));
        msg.msg_len  = strlen(msg.msg_data);
        msg.msg_type = 1;
        if (msg_send(writefd, &msg) < 0) {
            err_ret("message send failed");
        }
    } else {
        fprintf(stderr, "sending file: %s\n", msg.msg_data);
        times = 0;
        while ((n = read(fd, msg.msg_data, MSGDATA)) != 0) {
            msg.msg_len  = n;
            msg.msg_type = 1;
            if (msg_send(writefd, &msg) < 0) {
                err_ret("message send failed");
            }
            times++;
        }
        fprintf(stderr, "send times: %lu\n\n", times);
        close(fd);
    }

    msg.msg_len = 0;
    if (msg_send(writefd, &msg) < 0) {
        err_ret("mssage send failed:");
    }
}
