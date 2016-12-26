/*
 * test_filelock.c-- 模拟了一种会发生死锁的情况
 *
 * Copyright (C) 2014,2015,  <li_yunteng@163.com>
 * Auther: liyunteng  
 * License: GPL 
 * Update time:  2014/10/30 11:42:14
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
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>


static volatile sig_atomic_t sigflag; /* set nonzero by sig handler
					* */
static sigset_t newmask, oldmask, zeromask;

static void sig_user(int signo)	/* one signale handler for SIGUSR1
				 * and SIGUSR2 */
{
	sigflag = 1;
}

void TELL_WAIT(void)
{
	if (signal(SIGUSR1, sig_user) == SIG_ERR) {
		fprintf(stderr, "signal(SIGUSR1) error: %s\n", strerror(errno));
		return;
	}
	if (signal(SIGUSR2, sig_user) == SIG_ERR) {
		fprintf(stderr, "signal (SIGUSR2) error: %s\n", strerror(errno));
		return;
	}
	sigemptyset(&zeromask);
	sigemptyset(&newmask);
	sigaddset(&newmask, SIGUSR1);
	sigaddset(&newmask, SIGUSR2);

	/*
	 * Block SIGUSR1 and SIGUSR2, and save current signal mask.
	 */
	if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0) {
		fprintf(stderr, "SIG_BLOCK error: %s\n", strerror(errno));
		return;
	}
}

void TELL_PARENT(pid_t pid)
{
	kill(pid, SIGUSR2); 	/* tell parent we're done */
}

void WAIT_PARENT(void)
{
	while(sigflag == 0)
		sigsuspend(&zeromask);
	sigflag = 0;

	/*
	 * Reset signal mask to original value.
	 */
	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0) {
		fprintf(stderr, "SIG_SETMASK error: %s\n", strerror(errno));
		return;
	}
}

void TELL_CHILD(pid_t pid)
{
	kill(pid, SIGUSR1);	/* tell child we're done */
}

void WAIT_CHILD(void)
{
	while(sigflag == 0)
		sigsuspend(&zeromask); /* and wait for child */
	sigflag = 0;

	/*
	 * Reset signal mask to original value.
	 */
	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0){
		fprintf(stderr, "SIG_SETMASK error: %s\n", strerror(errno));
		return;
	}
}

static int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len)
{
	struct flock	lock;

	lock.l_type = type;
	lock.l_start = offset;
	lock.l_whence = whence;
	lock.l_len = len;

	return(fcntl(fd, cmd, &lock));
}

static void lockabyte(const char *name, int fd, off_t offset)
{
	if(lock_reg(fd, F_SETLKW, F_WRLCK, offset, SEEK_SET, 1) < 0) {
		fprintf(stderr, "%s: F_SETLKW lock error: %s\n", name, strerror(errno));
		return;
	}
	printf("%s: got the lock, byte %ld\n", name, offset);
}


int main(int argc, char *argv[])
{
	int	fd;
	pid_t	pid;

	/*
	 * Create a file and write two bytes to it.
	 */

	if ((fd = open("./tmplock", O_RDWR|O_CREAT, 644)) < 0) {
		fprintf(stderr, "create file error: %s\n", strerror(errno));
		return(errno);
	}

	if (write(fd, "ab", 2) != 2) {
		fprintf(stderr, "write error: %s\n", strerror(errno));
		return(errno);
	}

	TELL_WAIT();
	if ((pid = fork()) < 0 ) {
		fprintf(stderr, "fork error: %s\n", strerror(errno));
		return(errno);
	} else if (pid == 0) {
		lockabyte("child", fd, 0);
		TELL_PARENT(getppid()); 
		WAIT_PARENT();
		lockabyte("child", fd, 1);
	} else {
		lockabyte("parent", fd, 1);
		TELL_CHILD(pid);
		WAIT_CHILD();
		lockabyte("parent", fd, 0);
		pause();
	}
	return 0;
}










