/*
 * test_pthread_atfork.c--
 *
 * Copyright (C) 2014,2015,  <li_yunteng@163.com>
 * Auther: liyunteng  
 * License: GPL 
 * Update time:  2014/10/29 11:43:46
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
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

pthread_mutex_t	lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;

void prepare(void)
{
	printf("preparing locks...\n");
	pthread_mutex_lock(&lock1);
	pthread_mutex_lock(&lock2);
}

void parent(void)
{
	printf("parent unlocking locks...\n");
	pthread_mutex_unlock(&lock1);
	pthread_mutex_unlock(&lock2);
}

void child(void)
{
	printf("child unlock locks...\n");
	pthread_mutex_unlock(&lock1);
	pthread_mutex_unlock(&lock2);
}

void *thr_fn(void *arg)
{
	printf("thread stared ...\n");
	pause();
	return(0);
}

int main(int argc, char *argv[])
{
	int		err;
	pid_t		pid;
	pthread_t	tid;

	if ((err = pthread_atfork(prepare, parent, child)) != 0) {
		fprintf(stderr, "pthread_atfork failed: %s\n", strerror(err));
		return(err);
	}
	err = pthread_create(&tid, NULL, thr_fn, NULL);
	if (err != 0) {
		fprintf(stderr, "pthread_create failed: %s\n", strerror(err));
		return(err);
	}
	
	sleep(2);

	printf("parent about to fork...\n");
	if ((pid = fork()) < 0) {
		fprintf(stderr, "fork failed: %s\n", strerror(errno));
		return(errno);
	} else if (pid == 0) {
		printf("child returned from fork\n");
	} else {
		printf("parent return from fork\n");
	}
	
	return 0;
}






