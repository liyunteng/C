/*
 * daemon_reconfig.c--
 *
 * Copyright (C) 2014,2015,  <li_yunteng@163.com>
 * Auther: liyunteng  
 * License: GPL 
 * Update time:  2014/10/29 23:18:19
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
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <syslog.h>

sigset_t	mask;

extern int already_running(void);
extern void daemonize(const char *);

void reread(void)
{
	/* ... */
	
}

void *thr_fn(void *arge)
{
	int err, signo;

	for(;;) {
		err = sigwait(&mask, &signo);
		if (err !=0 ) {
			syslog(LOG_ERR, "sigwait failed");
			exit(1);
		}

		switch (signo) {
		case SIGHUP:
			syslog(LOG_INFO, "Re-reading configuration file");
			reread();
			break;

		case SIGTERM:
			syslog(LOG_INFO, "got SIGTERM; exiting");
			exit(0);
			
		default:
			syslog(LOG_INFO, "unexpected signal %d\n", signo);
		}
		
	}
	return(0);
}


int main(int argc, char *argv[])
{
	int		err;
	pthread_t	tid;
	char		*cmd;
	struct sigaction sa;

	if ((cmd = strrchr(argv[0], '/')) == NULL)
		cmd = argv[0];
	else
		cmd++;

	/*
	 *  Become a daemon.
	 */
	daemonize(cmd);

	/*
	 * Make sure only one copy of the daemon is running.
	 */
	if (already_running()) {
		syslog(LOG_ERR, "daemon already runnig");
		exit(1);
	}

	/*
	 * Restore SIGHUP default and block all signals.
	 */
	sa.sa_handler = SIG_DFL;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) < 0) {
		fprintf(stderr, "%s: can't restore SIGHUP default",cmd);
		exit(errno);
	}

	sigfillset(&mask);
	if ((err = pthread_sigmask(SIG_BLOCK, &mask, NULL)) != 0) {
		fprintf(stderr, "SIG_BLOCK error: %s", strerror(err));
		exit(err);
	}

	/*
	 * Create a thread to handle SIGHUP and SIGTERM
	 */
	err = pthread_create(&tid, NULL, thr_fn, NULL);
	if (err != 0) {
		fprintf(stderr, "can't create thread: %s", strerror(errno));
		exit(err);
	}

	/*
	 * Proceed with thre rest of the daemon.
	 */
	while(1) {
		syslog(LOG_ERR, "lyt");
		sleep(1);
	}
	return 0;
}







