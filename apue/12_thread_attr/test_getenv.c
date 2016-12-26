/*
 * test.c--
 *
 * Copyright (C) 2014,2015,  <li_yunteng@163.com>
 * Auther: liyunteng  
 * License: GPL 
 * Update time:  2014/10/29 11:51:25
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 * 
 */

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define MAXLINE 256

int getenv_r(const char *, char *, int);
char *getenv(const char *);

void *work(void *arg)
{
    //char buf[MAXLINE+1];
    //int err;
    char *buf;

    /* err = getenv_r("PWD", buf, MAXLINE); */
    /* if (err != 0) { */
    /*      fprintf(stderr, "tid %lu getenv_r failed: %s",  */
    /*              (unsigned long)pthread_self(), strerror(err)); */
    /*      pthread_exit((void *)err); */
    /* } */
    buf = getenv("PWD");
    if (buf == NULL) {
	fprintf(stderr, "tid %lu getenv failed!\n",
		(unsigned long) pthread_self());
    }
    printf("tid %lu pwd: %s\n", (unsigned long) pthread_self(), buf);
    pthread_exit((void *) 0);
}

int main(int argc, char *argv[])
{
    pthread_t tid1, tid2, tid3;
    int err;

    err = pthread_create(&tid1, NULL, work, NULL);
    if (err != 0) {
	fprintf(stderr, "create tid1 failed: %s\n", strerror(errno));
    }
    err = pthread_create(&tid2, NULL, work, NULL);
    if (err != 0) {
	fprintf(stderr, "create tid2 failed: %s\n", strerror(errno));
    }
    err = pthread_create(&tid3, NULL, work, NULL);
    if (err != 0) {
	fprintf(stderr, "create tid3 failed: %s\n", strerror(errno));
    }

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);
    return 0;
}
