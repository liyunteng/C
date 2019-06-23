/*
 * Description: ourhdr
 *
 * Copyright (C) 2019 liyunteng
 * Last-Updated: <2019/06/24 01:10:17>
 */
#ifndef OURHDR_H
#define OURHDR_H

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAXLINE 4096
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define DIR_MODE  (FILE_MODE | S_IXUSR | S_XGRP | S_IXOTH)

typedef void Sigfunc(int);

#if defined(SI_IGN) && !defined(SIG_ERR)
#define SIG_ERR ((Sigfunc *) -1)
#endif

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))


void err_dump(const char *, ...);
void err_msg(const char *, ...);
void err_quit(const char *, ...);
void err_ret(const char *, ...);
void err_sys(const char *, ...);

void log_msg(const char *, ...);
void log_open(const char *, int, int);
void log_quit(const char *, ...);
void log_ret(const char *, ...);
void log_sys(const char *, ...);

void TELL_WAIT(void);
void TELL_PARENT(pid_t);
void TELL_CHILD(pid_t);
void WAIT_PARENT(void);
void WAIT_CHILD(void);
void set_fl(int, int);
void clr_fl(int, int);
void pr_exit(int status);
void pr_mask(const char *str);

int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len);
pid_t lock_test(int fd, int type, off_t offset, int whence, off_t len);
#define read_lock(fd, offset, whence, len) \
    lock_reg(fd, F_SETLK, F_RDLCK, offset, whence, len)
#define readw_lock(fd, offset, whence, len) \
    lock_reg(fd, F_SETLKW, F_RDLCK, offset, whence, len)
#define write_lock(fd, offset, whence, len) \
    lock_reg(fd, F_SETLK, F_WRLCK, offset, whence, len)
#define writew_lock(fd, offset, whence, len) \
    lock_reg(fd, F_SETLKW, F_WRLCK, offset, whence, len)
#define un_lock(fd, offset, whence, len) \
    lock_reg(fd, F_SETLK, F_UNLCK, offset, whence, len)

ssize_t writen(int fd, const void *ptr, size_t n);
ssize_t readn(int fd, void *ptr, size_t n);
#endif
