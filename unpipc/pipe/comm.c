/*
 *更新时间: < 修改人[liyunteng] 2014/10/26 13:58:39 >
 */

#include "./comm.h"
#include "../unpipc.h"

ssize_t
msg_send(int fd, struct msg *mptr)
{
    return (write(fd, mptr, MSGHDRSIZE + mptr->msg_len));
}

ssize_t
msg_recv(int fd, struct msg *mptr)
{
    size_t  len;
    ssize_t n;

    if ((n = read(fd, mptr, MSGHDRSIZE)) == 0) {
        return (0);
    } else if (n != MSGHDRSIZE) {
        err_quit("message header: expected %d, get %d", MSGHDRSIZE, n);
    }

    if ((len = mptr->msg_len) > 0) {
        if ((n = read(fd, mptr->msg_data, len)) != len)
            err_quit("message data: expected %d, get %d", len, n);
    }
    return (len);
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

    if (msg_send(writefd, &msg) != (MSGHDRSIZE + msg.msg_len)) {
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

    msg.msg_type = 1;
    if ((n = msg_recv(readfd, &msg)) == 0) {
        return;
        err_ret("pathname missing");
    }

    msg.msg_data[n] = '\0';

    if ((fd = open(msg.msg_data, O_RDONLY)) < 0) {
        snprintf(msg.msg_data + n, sizeof(msg.msg_data) - n, ":can't open, %s\n", strerror(errno));
        msg.msg_len = strlen(msg.msg_data);
        if (msg_send(writefd, &msg) != (MSGHDRSIZE + msg.msg_len)) {
            err_ret("message send failed");
        }
    } else {
        fprintf(stderr, "sending file: %s\n", msg.msg_data);
        times = 0;
        while ((n = read(fd, msg.msg_data, MSGDATA)) != 0) {
            msg.msg_len = n;
            if (msg_send(writefd, &msg) != (MSGHDRSIZE + msg.msg_len)) {
                err_ret("message send failed");
            }
            times++;
        }
        fprintf(stderr, "send times: %lu\n\n", times);
        close(fd);
    }

    msg.msg_len = 0;
    if (msg_send(writefd, &msg) != (MSGHDRSIZE + msg.msg_len)) {
        err_ret("mssage send failed:");
    }
}
