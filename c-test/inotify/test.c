/*
 * test.c -- inotify test
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/11/27 07:05:35
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

#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>

static void handle_events(int fd, int *wd, int argc, char *argv[])
{
    char buf[4096]
	__attribute__ ((aligned(__alignof__(struct inotify_event))));
    const struct inotify_event *event;
    int i;
    ssize_t len;
    char *ptr;

    for (;;) {
	len = read(fd, buf, sizeof buf);
	if (len == -1 && errno != EAGAIN) {
	    perror("read");
	    exit(EXIT_FAILURE);
	}


	if (len <= 0)
	    break;

	for (ptr = buf; ptr < buf + len;
	     ptr += sizeof(struct inotify_event) + event->len) {
	    event = (const struct inotify_event *) ptr;

	    if (event->mask & IN_OPEN)
		printf("IN_OPEN: ");
	    if (event->mask & IN_CLOSE_NOWRITE)
		printf("IN_CLOSE_NOWRITE: ");
	    if (event->mask & IN_CLOSE_WRITE)
		printf("IN_CLOSE_WRITE: ");

	    for (i = 1; i < argc; ++i) {
		if (wd[i] == event->wd) {
		    printf("%s/", argv[i]);
		    break;
		}
	    }

	    if (event->len)
		printf("%s", event->name);


	    if (event->mask & IN_ISDIR)
		printf(" [directory]\n");
	    else
		printf(" [file]\n");
	}
    }
}

int main(int argc, char *argv[])
{
    char buf;
    int fd, i, poll_num;
    int *wd;
    nfds_t nfds;
    struct pollfd fds[2];

    if (argc < 2) {
	printf("Usage: %s PATH [PATH ...]\n", argv[0]);
	exit(EXIT_FAILURE);
    }

    printf("Press ENTER key to terminate.\n");

    fd = inotify_init1(IN_NONBLOCK);
    if (fd == -1) {
	perror("inotify_init1");
	exit(EXIT_FAILURE);
    }


    wd = calloc(argc, sizeof(int));
    if (wd == NULL) {
	perror("calloc");
	exit(EXIT_FAILURE);
    }

    for (i = 1; i < argc; i++) {
	wd[i] = inotify_add_watch(fd, argv[i], IN_OPEN | IN_CLOSE);
	if (wd[i] == -1) {
	    fprintf(stderr, "Cannot watch '%s'\n", argv[i]);
	    perror("inotify_add_watch");
	    exit(EXIT_FAILURE);
	}
    }

    nfds = 2;
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;

    fds[1].fd = fd;
    fds[1].events = POLLIN;

    printf("Listening for events.\n");
    while (1) {
	poll_num = poll(fds, nfds, -1);
	if (poll_num == -1) {
	    if (errno == EINTR)
		continue;
	    perror("poll");
	    exit(EXIT_FAILURE);
	}

	if (poll_num > 0) {
	    if (fds[0].revents & POLLIN) {
		while (read(STDIN_FILENO, &buf, 1) > 0 && buf != '\n')
		    continue;
		break;
	    }

	    if (fds[1].revents & POLLIN) {
		handle_events(fd, wd, argc, argv);
	    }
	}
    }

    printf("Listening for events stopped.\n");
    close(fd);
    free(wd);

    return 0;
}
