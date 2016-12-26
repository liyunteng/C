/*
 * test_signo.c--
 *
 * Copyright (C) 2014,2015,  <li_yunteng@163.com>
 * Auther: liyunteng  
 * License: GPL 
 * Update time:  2014/10/29 10:43:21
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
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <signal.h>


int		quitflag;	/* set nonzero by thread */
sigset_t	mask;

pthread_mutex_t	lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t	wait = PTHREAD_COND_INITIALIZER;

void *thr_fn(void *arg)
{
	int err, signo;

	for(;;) {
		err = sigwait(&mask, &signo);
		if (err != 0) {
			fprintf(stderr, "sigwait failed: %s\n",
				strerror(err));
			return((void *)err);
		}
		switch (signo) {
		case SIGINT:
			printf("interrupt\n");
			break;
			
		case SIGQUIT:
			pthread_mutex_lock(&lock);
			quitflag = 1;
			pthread_mutex_unlock(&lock);
			pthread_cond_signal(&wait);
			return(0);
			
		default:
			printf("unexpected signal %d\n", signo);
			exit(1);
		}

	}
}

int main(int argc, char *argv[])
{
	int		err;
	sigset_t	oldmask;
	pthread_t	tid;

	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask, SIGQUIT);
	sigaddset(&mask, SIGILL);

	if ((err = pthread_sigmask(SIG_BLOCK, &mask, &oldmask)) != 0) {
		fprintf(stderr, "SIG_BLOCK faield: %s\n", strerror(err));
		return(err);
	}

	err = pthread_create(&tid, NULL, thr_fn, NULL);
	if (err != 0) {
		fprintf(stderr, "pthread create failed: %s\n", strerror(err));
		return(err);
	}

	pthread_mutex_lock(&lock);
	while(quitflag == 0) {
		pthread_cond_wait(&wait, &lock);
	}
	pthread_mutex_unlock(&lock);
	
	printf("in main thread\n");
	
	quitflag = 0;

	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0) {
		fprintf(stderr, "SIG_SETMASK failed: %s\n", strerror(errno));
		return(errno);
	}
	
	return 0;
}
