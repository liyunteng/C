#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len);
#define read_lock(fd, offset, whence, len)              \
    lock_reg(fd, F_SETLK, F_RDLCK, offset, whence, len)
#define readw_lock(fd, offset, whence, len)                 \
    lock_reg(fd, F_SETLKW, F_RDLCK, offset, whence, len)
#define write_lock(fd, offset, whence, len)             \
    lock_reg(fd, F_SETLK, F_WRLCK, offset, whence, len)
#define writew_lock(fd, offset, whence, len)                \
    lock_reg(fd, F_SETLKW, F_WRLCK, offset, whence, len)
#define un_lock(fd, offset, whence, len)                \
    lock_reg(fd, F_SETLK, F_UNLCK, offset, whence, len)

int
lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len)
{
    struct flock lock;

    lock.l_type   = type;
    lock.l_start  = offset;
    lock.l_whence = whence;
    lock.l_len    = len;

    return (fcntl(fd, cmd, &lock));
}

void
set_fl(int fd, int flags)
{
    int val;

    if ((val = fcntl(fd, F_GETFL, 0)) < 0)
        err_sys("fcntl F_GETFL error");

    val |= flags;

    if (fcntl(fd, F_SETFL, val) < 0)
        err_sys("fcntl F_SETFL error");
}

void
clr_fl(int fd, int flags)
{
    int val;

    if ((val = fcntl(fd, F_GETFL, 0)) < 0)
        err_sys("fcntl F_GETFL error");

    val &= ~flags;

    if (fcntl(fd, F_SETFL, val) < 0)
        err_sys("fcntl F_SETFL error");
}


int
main(int argc, char *argv[])
{

    int          fd;
    struct flock lock;
    lock.l_type   = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start  = 0;
    lock.l_len    = 0;
    lock.l_pid    = getpid();

    if ((fd = open("/tmp/lyt.lock", O_CREAT | O_RDWR, 0666)) < 0) {
        fprintf(stderr, "create lock file failed.\n");
        return -1;
    }

    if (fcntl(fd, F_SETLK, &lock) < 0) {
        fprintf(stderr, "get file locak faile.\n");
        return -1;
    }

    printf("done");
    sleep(100);
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);

    return 0;
}
