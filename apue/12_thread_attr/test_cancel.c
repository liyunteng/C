/*
 * test_cancel.c--
 *
 * Copyright (C) 2014,2015,  <li_yunteng@163.com>
 * Auther: liyunteng  
 * License: GPL 
 * Update time:  2014/10/29 10:09:07
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
#include <time.h>

void *work(void *arg)
{
	struct timespec time;
	int	err, i = 0;
	
	time.tv_sec = 0;
	time.tv_nsec = 1;
	
	
	//err = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
	err = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	if (err != 0) {
		fprintf(stderr, "pthread_setcanclestate failed: %s\n",
			strerror(err));
		pthread_exit((void *)err);
	}

	/* 设置了异步类型，可以确保线程在任意时间取消。默认类型为延迟
	 * 取消 */
	err = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	if (err != 0) {
		fprintf(stderr, "pthread_setcanceltype failed: %s\n",
			strerror(err));
		pthread_exit((void *)err);
	}
	while(1) {
		//pthread_testcancel();
		printf("%d\n", i++);
		nanosleep(&time, NULL);
	}
}

int main(int argc, char *argv[])
{
	pthread_t	tid;
	int		err;
	struct timespec time;

	time.tv_sec = 0;
	time.tv_nsec = 100000;

	err = pthread_create(&tid, NULL, work, NULL);
	if (err != 0) {
		fprintf(stderr, "pthread_create failed: %s\n",
			strerror(errno));
		exit(errno);
	}
	
	nanosleep(&time, NULL);
	err = pthread_cancel(tid);
	if (err != 0) {
		fprintf(stderr, "pthread_cancel failed: %s\n",
			strerror(errno));
	}
	printf("cancel done\n");
	pthread_join(tid, NULL);
	
	return 0;
}
