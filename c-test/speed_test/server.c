/*
 * test.c --
 *
 * Copyright (C) 2016 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2016/04/08 16:55:47
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
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <inttypes.h>
#include <sys/time.h>
#include <signal.h>

#include "comm.h"

#define PORT 6666
#define MAX_THREAD 1024

pthread_t tids[MAX_THREAD];
typedef struct {
    int connfd;
    int id;
    struct sockaddr_in addr;
} thread_arg;
thread_arg args[MAX_THREAD];

void *thread(void *a)
{
    char buf[BUFFSIZE];
    thread_arg arg = *(thread_arg *) a;
    int n;
    unsigned long long count = 0;
    unsigned long long nrecv = 0;
    struct timeval tv1, tv2;

    fprintf(stderr, "connection from: %s:%d\n",
	    inet_ntoa(arg.addr.sin_addr), ntohs(arg.addr.sin_port));
    gettimeofday(&tv1, NULL);
    while ((n = recv(arg.connfd, buf, sizeof(buf), 0)) > 0) {
	count += n;
	nrecv++;
    }
    while (1) {
	int n = recv(arg.connfd, buf, sizeof(buf), 0);
	if (n > 0) {
	    count += n;
	    nrecv++;
	} else if (n == 0) {
	    //fprintf(stderr, "%s:%d closed\n", inet_ntoa(arg.addr.sin_addr), ntohs(arg.addr.sin_port));
	    break;
	} else {
	    fprintf(stderr, "recv failed: %s\n", strerror(errno));
	}
    }

    gettimeofday(&tv2, NULL);
    unsigned long long t =
	(tv2.tv_sec - tv1.tv_sec) * 1000 + (tv2.tv_usec -
					    tv1.tv_usec) / 1000;
    if (t == 0)
	t = 1;
    fprintf(stderr,
	    "%s:%d Upload size: %llu nrecv: %llu time: %llu speed: %llu kB/s\n",
	    inet_ntoa(arg.addr.sin_addr), ntohs(arg.addr.sin_port), count,
	    nrecv, t, count / t * 1000 / 1024);

    count = 0;
    unsigned long long nsent = 0;
    gettimeofday(&tv1, NULL);
    while (1) {
	memset(buf, 0xff, sizeof(buf));
	struct myhdr *hdr = (struct myhdr *) buf;
	hdr->type = MSG_SEND;
	hdr->seq = nsent;
	hdr->id = getpid();
	gettimeofday((struct timeval *) hdr->data, NULL);
	int n = sendto(arg.connfd, buf, sizeof(buf), 0, NULL, 0);
	if (n != sizeof(buf)) {
	    if (errno == ECONNRESET || errno == EPIPE) {
		nsent--;
		break;
	    } else if (errno == 0) {
		break;
	    }
	    fprintf(stderr, "send failed: %s\n", strerror(errno));
	} else {
	    count += sizeof(buf);
	    nsent++;
	}
    }
    gettimeofday(&tv2, NULL);
    t = (tv2.tv_sec - tv1.tv_sec) * 1000 + (tv2.tv_usec -
					    tv1.tv_usec) / 1000;
    if (t == 0)
	t = 1;
    if (nsent == 0)
	count = 0;
    fprintf(stderr,
	    "%s:%d Download size: %llu  nsent %llu time: %llu speed: %llu kB/s\n",
	    inet_ntoa(arg.addr.sin_addr), ntohs(arg.addr.sin_port), count,
	    nsent, t, count / t * 1000 / 1024);

    close(arg.connfd);
    tids[arg.id] = 0;

    return (void *) 0;
}

int tcp_speed_test(int port)
{
    struct sigaction sa;

    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    if (sigaction(SIGPIPE, &sa, 0) == -1) {
	fprintf(stderr, "sigaction failed: %s\n", strerror(errno));
	return -1;
    }


    int listenfd, connfd;
    struct sockaddr_in serveraddr;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	fprintf(stderr, "create socket failed: %s\n", strerror(errno));
	return -1;
    }

    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listenfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr))
	== -1) {
	fprintf(stderr, "bind failed: %s\n", strerror(errno));
	return -1;
    }

    if (listen(listenfd, 10) == -1) {
	fprintf(stderr, "listen failed: %s\n", strerror(errno));
	return -1;
    }

    while (1) {
	struct sockaddr_in cliaddr;
	socklen_t cliaddr_len = sizeof(cliaddr);
	if ((connfd =
	     accept(listenfd, (struct sockaddr *) &cliaddr,
		    &cliaddr_len)) == -1) {
	    fprintf(stderr, "accept failed: %s\n", strerror(errno));
	    continue;
	}

	int i;
	for (i = 0; i < MAX_THREAD; i++) {
	    if (tids[i] == 0)
		break;
	}
	args[i].id = i;
	args[i].connfd = connfd;
	args[i].addr = cliaddr;

	if (pthread_create(&tids[i], NULL, thread, (void *) &args[i]) != 0) {
	    perror("pthread_create failed");
	    close(connfd);
	    continue;
	}
    }

    int i;
    for (i = 0; i < MAX_THREAD; i++) {
	if (tids[i] != 0)
	    pthread_join(tids[i], NULL);
    }

    close(listenfd);
    return 0;

}

int main(int argc, char *argv[])
{
    int port = PORT;
    if (argc == 2)
	port = atoi(argv[1]);

    tcp_speed_test(port);
    return 0;
}
