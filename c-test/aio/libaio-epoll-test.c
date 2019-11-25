/*
 * libaio-epoll-test.c - libaio-epoll-test
 *
 * Date   : 2019/11/22
 */
#include <assert.h>
#include <inttypes.h>
#include <libaio.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define __USE_GNU 1
#include <fcntl.h>

#define FILE_NAME "test.txt"
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define FILE_SIZE 128 * 1024
#define BUF_SIZE 512
#define MAX_EVENTS 128

struct custom_iocb {
    struct iocb iocb;
    int         nth_request;
};

void
aio_callback(io_context_t ctx, struct iocb *iocb, long res, long res2)
{
    struct custom_iocb *ciocb = (struct custom_iocb *)iocb;
    printf(
        "##nth_request: %d, request_type: %s, offset: %lld, length: %lu, res: %ld, res2: %ld##\n",
        ciocb->nth_request, (ciocb->iocb.aio_lio_opcode == IO_CMD_PREAD) ? "READ" : "WRITE",
        ciocb->iocb.u.c.offset, ciocb->iocb.u.c.nbytes, res, res2);

    char buf[BUF_SIZE + 1];
    snprintf(buf, BUF_SIZE + 1, "%s", ciocb->iocb.u.c.buf);
    printf("%s", buf);
}

void
test(const char *filename)
{
    int                 efd, fd, epfd;
    io_context_t        ctx;
    struct timespec     tms;
    struct io_event     events[MAX_EVENTS];
    struct iocb *       iocbs[MAX_EVENTS];
    struct custom_iocb  ciocbs[MAX_EVENTS];
    struct custom_iocb *ciocbp;

    int                i, j, rc;
    void *             buf;
    struct epoll_event epevent;

    efd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    assert(efd > 0);

    fd = open(filename, O_RDWR | O_CREAT | O_DIRECT, FILE_MODE);
    assert(fd > 0);
    /* ftruncate(fd, FILE_SIZE); */

    ctx = 0;
    rc  = io_setup(MAX_EVENTS, &ctx);
    assert(rc == 0);

    for (i = 0, ciocbp = ciocbs; i < MAX_EVENTS; ++i, ++ciocbp) {
        buf = NULL;
        rc  = posix_memalign(&buf, getpagesize(), BUF_SIZE);
        assert(rc == 0 && buf != NULL);

        iocbs[i] = &ciocbp->iocb;
        io_prep_pread(&ciocbp->iocb, fd, buf, BUF_SIZE, i * BUF_SIZE);
        io_set_eventfd(&ciocbp->iocb, efd);
        io_set_callback(&ciocbp->iocb, aio_callback);
        ciocbp->nth_request = i + 1;
    }

    rc = io_submit(ctx, MAX_EVENTS, iocbs);
    assert(rc == MAX_EVENTS);

    epfd = epoll_create(1);
    assert(epfd > 0);

    epevent.events   = EPOLLIN | EPOLLET;
    epevent.data.ptr = NULL;
    rc               = epoll_ctl(epfd, EPOLL_CTL_ADD, efd, &epevent);
    assert(rc == 0);

    i = 0;
    while (i < MAX_EVENTS) {
        uint64_t finished_aio;

        rc = epoll_wait(epfd, &epevent, 1, -1);
        assert(rc == 1);

        rc = read(efd, &finished_aio, sizeof(finished_aio));
        assert(rc == sizeof(finished_aio));

        printf("finished io number: %" PRIu64 "\n", finished_aio);

        while (finished_aio > 0) {
            tms.tv_sec  = 0;
            tms.tv_nsec = 0;
            rc          = io_getevents(ctx, 1, MAX_EVENTS, events, &tms);
            assert(rc >= 0);
            for (j = 0; j < rc; j++) {
                ((io_callback_t)(events[j].data))(ctx, events[j].obj, events[j].res,
                                                  events[j].res2);
            }
            i += rc;
            finished_aio -= rc;
        }
    }

    close(epfd);
    free(buf);
    io_destroy(ctx);
    close(fd);
    close(efd);
}

int
main(int argc, char *argv[])
{
    char *filename = FILE_NAME;
    if (argc == 2) {
        filename = argv[1];
    }
    test(filename);
    return 0;
}

/* Local Variables: */
/* compile-command: "clang -Wall -o libaio-epoll-test libaio-epoll-test.c -g -laio" */
/* End: */
