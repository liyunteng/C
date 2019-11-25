/*******************************************************************************
 * Author : liyunteng
 * Email : li_yunteng@163.com
 * Created Time : 2014-02-20 15:13
 * Filename : mkdir_p.c
 * Description :
 * *****************************************************************************/
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "debug.h"
#include "jw-unistd.h"

int
mkdir_p(const char *path)
{
    int  n;
    char buf[PATH_MAX], *p = buf;

    assert(path);

    if ((n = strlen(path)) > PATH_MAX) {
        DBGP("over length: %d, %d", n, PATH_MAX);
        return -1;
    }

    DBGP("len=%d; path='%s'\n", n, path);

    strcpy(buf, path);
    while ((p = strchr(p + 1, '/'))) {
        struct stat sb;
        *p = '\0';
        if (stat(buf, &sb) && mkdir(buf, ACCESSPERMS)) {
            DBGE("stat/create '%s' failed", buf);
            return -1;
        }
        *p = '/';
    }

    return n;
}
