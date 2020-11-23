/*
 * other.c - other
 *
 * Author : liyunteng <liyunteng@streamocean.com>
 * Date   : 2019/06/23
 *
 * Copyright (C) 2019 StreamOcean, Inc.
 * All rights reserved.
 */
#include "ourhdr.h"
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

static volatile sig_atomic_t sigflag;
static sigset_t newmask;
static sigset_t oldmask;
static sigset_t zeromask;

static void
sig_usr(int signo)
{
    sigflag = 1;
}
void
TELL_WAIT(void)
{
    if (signal(SIGUSR1, sig_usr) == SIG_ERR)
        err_sys("signal error");
    if (signal(SIGUSR2, sig_usr) == SIG_ERR)
        err_sys("signal error");

    sigemptyset(&zeromask);
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGUSR1);
    sigaddset(&newmask, SIGUSR2);

    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
        err_sys("SIG_BLOCK error");
}

void
TELL_PARENT(pid_t pid)
{
    kill(pid, SIGUSR2);
}

void
TELL_CHILD(pid_t pid)
{
    kill(pid, SIGUSR1);
}

void
WAIT_PARENT(void)
{
    while (sigflag == 0)
        sigsuspend(&zeromask);

    sigflag = 0;
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
        err_sys("SIG_SETMASK error");
}

void
WAIT_CHILD(void)
{
    while (sigflag == 0)
        sigsuspend(&zeromask);
    sigflag = 0;
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
        err_sys("SIG_SETMASK error");
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

void
pr_exit(int status)
{
    if (WIFEXITED(status))
        printf("normal termination, exit status = %d\n", WEXITSTATUS(status));
    else if (WIFSIGNALED(status))
        printf("abnormal termination, signal number = %d(%s)\n", WTERMSIG(status), strsignal(WTERMSIG(status)),
#ifdef WCOREDUMP
               WCOREDUMP(status) ? " (core file generated)" : ""
#else
               ""
#endif  // WCOREDUMP
        );
    else if (WIFSTOPPED(status))
        printf("child stopped, signal number = %d\n", WSTOPSIG(status));
}

void
pr_mask(const char *str)
{
    sigset_t sigset;
    int errno_save;

    errno_save = errno;
    if (sigprocmask(0, NULL, &sigset) < 0)
        err_sys("sigprocmask error");

    printf("%s", str);
    if (sigismember(&sigset, SIGINT))
        printf("SIGINT ");
    if (sigismember(&sigset, SIGQUIT))
        printf("SIGQUIT ");
    if (sigismember(&sigset, SIGUSR1))
        printf("SIGUSR1 ");
    if (sigismember(&sigset, SIGALRM))
        printf("SIGALRM");

    printf("\n");
    errno = errno_save;
}

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

pid_t
lock_test(int fd, int type, off_t offset, int whence, off_t len)
{
    struct flock lock;

    lock.l_type   = type;
    lock.l_start  = offset;
    lock.l_whence = whence;
    lock.l_len    = len;

    if (fcntl(fd, F_GETLK, &lock) < 0)
        err_sys("fcntl error");

    if (lock.l_type == F_UNLCK)
        return (0);

    return (lock.l_pid);
}

ssize_t
writen(int fd, const void *vptr, size_t n)
{
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;

    ptr   = vptr;
    nleft = n;
    while (nleft > 0) {
        if ((nwritten = write(fd, ptr, nleft)) <= 0)
            return (nwritten);
        nleft -= nwritten;
        ptr += nwritten;
    }

    return (n);
}

ssize_t
readn(int fd, void *vptr, size_t n)
{
    size_t nleft;
    ssize_t nread;
    char *ptr;

    ptr   = vptr;
    nleft = n;
    while (nleft > 0) {
        if ((nread = read(fd, ptr, nleft)) < 0)
            return (nread);
        else if (nread == 0)
            break;

        nleft -= nread;
        ptr += nread;
    }

    return (n - nleft);
}
