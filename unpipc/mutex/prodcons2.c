/*
 * prodcons2.c--
 *
 * Copyright (C) 2014,2015,  <li_yunteng@163.com>
 * Auther: liyunteng
 * License: GPL
 * Update time:
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

#include "../unpipc.h"
#include <pthread.h>

#define MAXITEMS	1000000
#define MAXTHREADS	100

int nitems;

struct {
    pthread_mutex_t mutex;
    int buf[MAXITEMS];
    int nput;
    int nval;
} shared = {
PTHREAD_MUTEX_INITIALIZER};

void *produce(void *), *consume(void *);

inline int min(int a, int b)
{
    return (a > b ? b : a);
}

int main(int argc, char *argv[])
{
    int i, nthreads, count[MAXTHREADS];
    pthread_t tid_produce[MAXTHREADS], tid_consume;

    if (argc != 3)
	err_quit("usage: %s <#items> <#threads>\n", argv[0]);
    nitems = min(atoi(argv[1]), MAXITEMS);
    nthreads = min(atoi(argv[2]), MAXTHREADS);

    if (pthread_setconcurrency(nthreads) < 0)
	err_sys("pthread_setconcurrency error: ");

    for (i = 0; i < nthreads; i++) {
	count[i] = 0;
	if (pthread_create(&tid_produce[i], NULL, produce, &count[i]) < 0)
	    err_sys("pthread_create produce %ld error: ", i);
    }

    for (i = 0; i < nthreads; i++) {
	if (pthread_join(tid_produce[i], NULL) < 0)
	    err_sys("pthread_join %ld error: ", i);
	printf("count[%d] = %d\n", i, count[i]);
    }

    if (pthread_create(&tid_consume, NULL, consume, NULL) < 0)
	err_sys("pthread_create consume error: ");
    if (pthread_join(tid_consume, NULL) < 0)
	err_sys("pthread_join consume error: ");
    return 0;
}

void *produce(void *arg)
{
    for (;;) {
	if (pthread_mutex_lock(&shared.mutex) < 0)
	    err_sys("pthread_mutex_lock %ld lock error: ",
		    (long) pthread_self());
	if (shared.nput >= nitems) {
	    if (pthread_mutex_unlock(&shared.mutex) < 0)
		err_sys("pthread_mutex_unlock %ld unlock error: ",
			(long) pthread_self());
	    return (NULL);
	}

	shared.buf[shared.nput] = shared.nval;
	shared.nput++;
	shared.nval++;
	if (pthread_mutex_unlock(&shared.mutex) < 0)
	    err_sys("pthread_mutex_unlock %ld unlock error: ",
		    (long) pthread_self());
	*((int *) arg) += 1;
    }
}

void *consume(void *arg)
{
    int i;

    for (i = 0; i < nitems; i++) {
	if (shared.buf[i] != i)
	    printf("buf[%d] = %d\n", i, shared.buf[i]);
    }
    return (NULL);
}
