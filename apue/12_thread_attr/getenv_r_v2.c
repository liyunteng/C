/*
 * getenv_r_v2.c--
 *
 * Copyright (C) 2014,2015,  <li_yunteng@163.com>
 * Auther: liyunteng
 * License: GPL
 * Update time:  2016/11/30 14:24:05
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

#include <limits.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

#define ARG_MAX 256
/* 为每个线程malloc一个单独的buf，并使用pthread_key_t来保存，然后返回
 * 每个线程的私有数据，因此不会产生混乱 */

static pthread_key_t key;
static pthread_once_t init_done = PTHREAD_ONCE_INIT;
pthread_mutex_t env_mutex = PTHREAD_MUTEX_INITIALIZER;

extern char **environ;

static void thread_init(void)
{
    pthread_key_create(&key, free);
}

char *getenv(const char *name)
{
    int i, len;
    char *envbuf;

    pthread_once(&init_done, thread_init);
    pthread_mutex_lock(&env_mutex);
    envbuf = (char *) pthread_getspecific(key);	/* 创建私有数据 */
    if (envbuf == NULL) {
	envbuf = malloc(ARG_MAX);	/* 每个线程单独申请内存空间 */
	if (envbuf == NULL) {
	    pthread_mutex_unlock(&env_mutex);
	    return (NULL);
	}
	pthread_setspecific(key, envbuf);	/* 将每个线程的地址
						 * 空间和key绑定，因
						 * 此需要使用mutex */
    }
    len = strlen(name);
    for (i = 0; environ[i] != NULL; i++) {
	if ((strncmp(name, environ[i], len) == 0) &&
	    (environ[i][len] == '=')) {
	    strcpy(envbuf, &environ[i][len + 1]);
	    pthread_mutex_unlock(&env_mutex);
	    return (envbuf);
	}
    }
    pthread_mutex_unlock(&env_mutex);


    return (NULL);
}
