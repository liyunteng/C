#ifndef UNPIPC__H__
#define UNPIPC__H__

#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <error.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#include <stdarg.h>
#include <syslog.h>
#include <limits.h>


#define MAXLINE 1024

int daemon_proc;

static inline void err_doit(int errnoflag, int level, const char *fmt, va_list ap)
{
	int errno_save, n;
	char buf[MAXLINE];

	errno_save = errno;
	vsnprintf(buf, sizeof(buf), fmt, ap);
	n = strlen(buf);
	if (errnoflag) {
		snprintf(buf+n, sizeof(buf)-n, ": %s\n", strerror(errno_save));
		
	}

	if (daemon_proc){
		syslog(level, "%s", buf);
	} else {
		fflush(stdout);
		fputs(buf, stderr);
		fflush(stderr);
	}
}

static inline void err_ret(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(1, LOG_INFO, fmt, ap);
	va_end(ap);
	return;
	
}

static inline void err_sys(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(1, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(1);
}

static inline void err_dump(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(1, LOG_ERR, fmt, ap);
	va_end(ap);
	abort();
	exit(1);
}

static inline void err_msg(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(0, LOG_INFO, fmt, ap);
	va_end(ap);
	return;
}

static inline void err_quit(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(0, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(1);
}


#endif // UNPIPC__H__
