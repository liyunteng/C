/*
 * sigtstp.c--
 *
 * Copyright (C) 2014,2015,  <li_yunteng@163.com>
 * Auther: liyunteng
 * License: GPL
 * Update time:  2016/12/21 16:27:21
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
#include <signal.h>

#define BUFFSIZE	1024

static void sig_tstp(int);

int main(int argc, char *argv[])
{
    int n;
    char buf[BUFFSIZE];

    if (signal(SIGTSTP, SIG_IGN) == SIG_DFL)
	signal(SIGTSTP, sig_tstp);

    while ((n = read(STDIN_FILENO, buf, BUFFSIZE)) > 0)
	if (write(STDOUT_FILENO, buf, n) != n)
	    fprintf(stderr, "write error");

    if (n < 0)
	fprintf(stderr, "read error");
    return 0;
}

static void sig_tstp(int signo)
{
    sigset_t mask;

    sigemptyset(&mask);
    sigprocmask(SIG_UNBLOCK, &mask, NULL);


    signal(SIGTSTP, SIG_DFL);

    kill(getpid(), SIGTSTP);

    signal(SIGTSTP, sig_tstp);
}
