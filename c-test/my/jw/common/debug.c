/*******************************************************************************
* Author : liyunteng
* Email : li_yunteng@163.com
* Created Time : 2014-02-20 15:45
* Filename : debug.c
* Description : 
* *****************************************************************************/

#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

#include "debug.h"

static bool bstamp = true;

void set_dbgstamp(bool stamp)
{
    bstamp = stamp;
}

bool get_dbgstamp()
{
    return bstamp;
}

void __dbg_vprintf(FILE * stream,
		   const char *file,
		   const char *func,
		   size_t line, bool berr, const char *fmt, va_list ap)
{
    char buf[32];
    size_t i = 0;
    static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

    if (bstamp) {
	time_t now = time(NULL);
	struct tm now_tm;
	i = strftime(buf, 31, "%h %e %T", localtime_r(&now, &now_tm));
    }

    buf[i] = '\0';


    pthread_mutex_lock(&lock);

    fprintf(stream, "%s %s, %zu, %s:", buf, file, line, func);
    vprintf(stream, fmt, ap);
    if (berr) {
	fprintf(stream, ": %s\n", strerror(errno));
    } else if (*fmt) {
	i = strlen(fmt);
	--i;
	if (fmt[i] != '\n')
	    fprintf(stream, "\n");
    }

    pthread_mutex_unlock(&lock);
}
