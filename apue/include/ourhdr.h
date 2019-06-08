/*
 * Description: ourhdr
 *
 * Copyright (C) 2019 liyunteng
 * Last-Updated: <2019/06/08 10:06:56 liyunteng>
 */
#ifndef OURHDR_H
#define OURHDR_H

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAXLINE 4096
#define FILE_MODE (S_IRUSR | S_IRGRP | S_IROTH)
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
#endif
