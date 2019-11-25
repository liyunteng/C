/*
 * libaio-test.c - libaio-test
 *
 * Date   : 2019/11/22
 */
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libaio.h>
#include <unistd.h>
#include <assert.h>

#define AIO_BLKSIZE 4096
#define AIO_MAXIO 64

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define FILE_NAME "test.txt"

static void rd_done(io_context_t ctx, struct iocb *iocb, long res, long res2)
{
    int iosize = iocb->u.c.nbytes;
    char *buf = (char *)iocb->u.c.buf;

    printf("iosize: %d, res: %ld, res2: %ld\n", iosize, res, res2);
    assert(res2 == 0 && res >= 0);

    printf("%s", buf);
}

static void wr_done(io_context_t ctx, struct iocb *iocb, long res, long res2)
{
    int iosize = iocb->u.c.nbytes;
    assert(iosize == res && res2 == 0);
}

void read_test(const char *filename)
{
    io_context_t ctx;
    int rc;
    char *buf = NULL;
    int fd;

    fd = open(filename, O_RDONLY);
    assert(fd > 0);

    memset(&ctx, 0, sizeof(ctx));
    io_queue_init(AIO_MAXIO,&ctx);

    struct iocb*iocb = (struct iocb*)malloc(sizeof(struct iocb));
    assert(iocb != NULL);
    int tmp = posix_memalign((void **)&buf, getpagesize(),AIO_BLKSIZE);
    assert(tmp == 0 && buf != NULL);

    io_prep_pread(iocb, fd, buf, AIO_BLKSIZE, 0);
    io_set_callback(iocb, rd_done);
    rc = io_submit(ctx, 1, &iocb);
    assert(rc == 1);

    io_queue_run(ctx);

    free(buf);
    io_queue_release(ctx);
    close(fd);
}

void write_test(const char *filename)
{
    int fd;
    char buf[] = "This is a libaio test.\n";
    io_context_t ctx;
    struct iocb* iocb = NULL;
    int rc;

    fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, FILE_MODE);
    assert(fd > 0);

    memset(&ctx, 0, sizeof(ctx));
    io_setup(AIO_MAXIO, &ctx);

    iocb = (struct iocb*)malloc(sizeof(struct iocb));
    assert(iocb != NULL);

    iocb->aio_fildes = fd;
    iocb->aio_lio_opcode = IO_CMD_PWRITE;
    iocb->aio_reqprio = 0;
    iocb->u.c.buf = buf;
    iocb->u.c.nbytes = strlen(buf);

    io_set_callback(iocb, wr_done);
    rc = io_submit(ctx,1,&iocb);
    assert(rc == 1);

#if 1
    struct io_event events[AIO_MAXIO];
    io_callback_t cb;
    int num = io_getevents(ctx, 1, AIO_MAXIO, events, NULL);
    printf("%d io_request completed\n", num);

    for (int i = 0; i < num; i++) {
        cb = (io_callback_t)events[i].data;
        struct iocb *io = events[i].obj;

        printf("events[%d].data=%p, res=%ld, res2=%ld\n",
               i, cb, events[i].res, events[i].res2);
        cb(ctx, io, events[i].res, events[i].res2);
    }
#endif
    io_destroy(ctx);
    close(fd);
}

int main(int argc, char *argv[])
{
    char *filename = FILE_NAME;
    if (argc == 2) {
        filename = argv[1];
    }
    write_test(filename);
    read_test(filename);
    return 0;
}

/* Local Variables: */
/* compile-command: "clang -Wall -o libaio-test libaio-test.c -g -laio" */
/* End: */
