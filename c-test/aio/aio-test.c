/*
 * aio-test.c - aio test
 *
 * Date   : 2019/11/20
 */

#include <aio.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define FILE_NAME "test.txt"
#define BUFFER_SIZE 4096
#define MAX_LIST 2

void
aio_handler(__sigval_t sigval)
{
    struct aiocb *prd;
    int ret;

    prd = (struct aiocb *)sigval.sival_ptr;

    if (aio_error(prd) == 0) {
        ret = aio_return(prd);
        printf("%s", prd->aio_buf);
        printf("read: %d\n", ret);
    }
}
int
async_test(const char *filename)
{
    int fd, ret;
    struct aiocb rd;

    fd = open(filename, O_RDONLY);
    assert(fd > 0);

    memset(&rd, 0, sizeof(rd));
    rd.aio_fildes = fd;
    rd.aio_buf    = (char *)malloc(BUFFER_SIZE);
    rd.aio_nbytes = BUFFER_SIZE;
    rd.aio_offset = 0;

    rd.aio_sigevent.sigev_notify            = SIGEV_THREAD; /* CALLBACK */
    rd.aio_sigevent.sigev_notify_function   = aio_handler;
    rd.aio_sigevent.sigev_notify_attributes = NULL;
    rd.aio_sigevent.sigev_value.sival_ptr   = &rd;

    ret = aio_read(&rd);
    assert(ret == 0);

    sleep(1); /* WAIT CALLBACK */
    return 0;
}
int
lio_test(const char *filename)
{
    struct aiocb *listio[MAX_LIST];
    struct aiocb rd, wr;
    int fd, ret;
    char str[] = "This is a lio test.\n";

    fd = open(filename, O_RDWR | O_APPEND | O_CREAT, FILE_MODE);
    assert(fd > 0);

    memset(&wr, 0, sizeof(wr));
    wr.aio_buf = str;
    assert(wr.aio_buf != NULL);
    wr.aio_fildes     = fd;
    wr.aio_nbytes     = strlen(str);
    wr.aio_lio_opcode = LIO_WRITE; /* WRITE */
    listio[0]         = &wr;

    memset(&rd, 0, sizeof(rd));
    rd.aio_buf = (char *)malloc(BUFFER_SIZE);
    assert(rd.aio_buf != NULL);
    rd.aio_fildes     = fd;
    rd.aio_nbytes     = BUFFER_SIZE;
    rd.aio_offset     = 0;
    rd.aio_lio_opcode = LIO_READ; /* READ */

    listio[1] = &rd;

    ret = lio_listio(LIO_WAIT, listio, MAX_LIST, NULL);
    ret = aio_return(&wr);
    printf("write: %d\n", ret);
    ret = aio_return(&rd);
    printf("%s", rd.aio_buf);
    printf("read: %d\n", ret);

    return 0;
}

int
suspend_test(const char *filename)
{
    struct aiocb rd;
    int fd, ret;
    const struct aiocb *aiocb_list[MAX_LIST];
    fd = open(filename, O_RDONLY);
    assert(fd > 0);

    memset(&aiocb_list, 0, sizeof(aiocb_list));
    memset(&rd, 0, sizeof(rd));
    rd.aio_buf    = malloc(BUFFER_SIZE);
    rd.aio_fildes = fd;
    rd.aio_nbytes = BUFFER_SIZE;
    rd.aio_offset = 0;

    aiocb_list[0] = &rd;

    ret = aio_read(&rd);
    assert(ret >= 0);

    ret = aio_suspend(aiocb_list, sizeof(aiocb_list) / sizeof(aiocb_list[0]),
                      NULL);
    printf("%s", rd.aio_buf);
    free((void *)rd.aio_buf);
    close(fd);
    return 0;
}

int
read_test(const char *filename)
{
    struct aiocb rd;
    int fd, ret, counter;
    fd = open(filename, O_RDONLY);
    assert(fd > 0);

    memset(&rd, 0, sizeof(rd));
    rd.aio_buf    = malloc(BUFFER_SIZE);
    rd.aio_fildes = fd;
    rd.aio_nbytes = BUFFER_SIZE;
    rd.aio_offset = 0;

    ret = aio_read(&rd);
    assert(ret == 0);

    counter = 0;
    while (aio_error(&rd) == EINPROGRESS) {
        /* printf("%d retry\n", ++counter); */
    }

    ret = aio_return(&rd);
    printf("ret: %d\n", ret);
    printf("%s", rd.aio_buf);
    free((void *)rd.aio_buf);
    close(fd);
    return 0;
}

int
write_test(const char *filename)
{
    struct aiocb wr;
    int ret, fd, counter;
    char str[] = "This is a test txt.\n";
    fd         = open(filename, O_CREAT | O_WRONLY | O_APPEND, FILE_MODE);
    assert(fd > 0);

    memset(&wr, 0, sizeof(wr));
    wr.aio_buf    = str;
    wr.aio_fildes = fd;
    wr.aio_nbytes = strlen(str);

    ret = aio_write(&wr);
    assert(ret == 0);

    counter = 0;
    while (aio_error(&wr) == EINPROGRESS) {
        /* printf("%d retry\n", ++counter); */
    }

    ret = aio_return(&wr);
    printf("ret: %d\n", ret);
    close(fd);
    return 0;
}

int
main(int argc, char *argv[])
{
    const char *filename = FILE_NAME;
    if (argc == 2) {
        filename = argv[1];
    }
    printf("write_test\n");
    write_test(filename);
    printf("read_test\n");
    read_test(filename);
    printf("suspend_test\n");
    suspend_test(filename);
    printf("lio_test\n");
    lio_test(filename);
    printf("async_test\n");
    async_test(filename);
    return 0;
}

/* Local Variables: */
/* compile-command: "clang -Wall -o aio-test aio-test.c -g -lrt" */
/* End: */
