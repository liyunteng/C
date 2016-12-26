/*
 *更新时间: < 修改人[liyunteng] 2014/10/25 12:35:58 >
 */

#include "../unp.h"
#include "./error.h"

#include <stdarg.h>
#include <syslog.h>

int daemon_proc;

static void err_doit(int errnoflag, int level, const char *fmt, va_list ap)
{
	int	errno_save, n;
	char	buf[MAXLINE+1];

	errno_save = errno;
#ifdef HAVE_VSNPRINTF
	vsnprintf(buf, MAXLINE, fmt, ap);
#else
	vsprintf(buf, fmt, ap);
#endif // HAVE_VSNPRINTF

	n = strlen(buf);
	if (errnoflag) {
		syslog(level, buf);
	}
	
	fflush(stdout);
	fputs(buf, stderr);
	fflush(stderr);
	
	return;
}


void err_ret(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_doit(1, LOG_INFO, fmt, ap);
	va_end(ap);
	return;
}

void err_sys(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_doit(1, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(1);
}

void err_dump(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_doit(1, LOG_ERR, fmt, ap);
	va_end(ap);
	abort();
	exit(1);
}

void err_msg(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_doit(0, LOG_INFO, fmt, ap);
	va_end(ap);
	return;
}

void err_quit(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_doit(0, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(1);
}



