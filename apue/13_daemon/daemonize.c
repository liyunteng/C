/*
 * daemonize.c--
 *
 * Copyright (C) 2014,2015,  <li_yunteng@163.com>
 * Auther: liyunteng  
 * License: GPL 
 * Update time:  2014/10/29 23:41:25
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
#include <errno.h>
#include <syslog.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/resource.h>

void daemonize(const char *cmd)
{
	int		fd0, fd1, fd2;
	unsigned	i;
	pid_t		pid;
	struct rlimit	rl;
	struct sigaction sa;

	/*
	 * Clear file creation mask.
	 */
	umask(0);

	/*
	 * Get maximum number of file descriptors.
	 */
	if (getrlimit(RLIMIT_NOFILE, &rl) < 0) {
		fprintf(stderr, "%s: can't get file limit.\n", cmd);
		return;
	}

	/*
	 * Become a session leader to lose controlling TTY.
	 */
	if ((pid = fork()) < 0)  {
		fprintf(stderr, "%s: can't fork\n", cmd);
		return;
	} else if (pid != 0) {
		exit(0);	/* parent quit */
	}
	setsid();

	/*
	 * Ensure future opens won't allocate contolling TTYS.
	 */
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) < 0) {
		fprintf(stderr, "%s: can't ignore SIGHUP.\n", cmd);
		return;
	}
	if ((pid = fork()) < 0)  {
		fprintf(stderr,"%s: can't fork.\n", cmd);
		return;
	} else if (pid != 0) {
		exit(0);
	}

	/*
	 * Change the current working directory to the root so we
	 * won't prevent file systems from being unmounted.
	 */
	if (chdir("/") < 0) {
		fprintf(stderr,"%s: can't change directory to /.\n", cmd);
		return;
	}

	/*
	 * Close all open file descriptions.
	 */
	if (rl.rlim_max == RLIM_INFINITY)
		rl.rlim_max = 1024;
	for (i = 0; i < rl.rlim_max; i++) {
		close(i);
	}

	/*
	 * Attach file descriptions 0, 1, and 2 to /dev/null.
	 */
	fd0 = open("/dev/null", O_RDWR);
	fd1 = dup(0);
	fd2 = dup(0);

	/*
	 * Initialize the log file.
	 */
	openlog(cmd, LOG_CONS, LOG_DAEMON);
	if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
		syslog(LOG_ERR, "unexpected file descriptions %d %d %d",
		       fd0, fd1, fd2);
		exit(1);
	}

	
	/* 
         * while(1) {
	 * 	syslog(LOG_INFO, "%s: running...", cmd);
	 * 	sleep(1);
	 * }
         */
	
		
}

