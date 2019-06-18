/*
 * 7-7.c - 7-7 print resource limits
 *
 * Author : liyunteng <li_yunteng@163.com>
 * Date   : 2019/06/19
 */
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "ourhdr.h"


#define doit(name) pr_limits(#name, name)
static void pr_limits(char *, int);

int main(void)
{
    doit(RLIMIT_CORE);
    doit(RLIMIT_CPU);
    doit(RLIMIT_DATA);
    doit(RLIMIT_FSIZE);

#ifdef RLIMIT_MEMLOCK
    doit(RLIMIT_MEMLOCK);
#endif // RLIMIT_MEMLOCK

#ifdef RLIMIT_OFILE
    doit(RLIMIT_OFILE);
#endif // RLIMIT_OFILE

#ifdef RLIMIT_NOFILE
    doit(RLIMIT_NOFILE);
#endif // RLIMIT_NOFILE

#ifdef RLIMIT_NPROC
    doit(RLIMIT_NPROC);
#endif // RLIMIT_NPROC

#ifdef RLIMIT_RSS
    doit(RLIMIT_RSS);
#endif // RLIMIT_RSS

    doit(RLIMIT_STACK);

#ifdef RLIMIT_VMEM
    doit(RLIMIIT_VMEM)
#endif // RLIMIT_VMEM

    return 0;
}

static void pr_limits(char *name, int resource)
{
    struct rlimit limit;
    if (getrlimit(resource, &limit) < 0) {
        err_sys("getrlimit error for %s", name);
    }
    printf("%-14s  ", name);
    if (limit.rlim_cur == RLIM_INFINITY)
        printf("(infinite)  ");
    else
        printf("%10ld  ", limit.rlim_cur);
    if (limit.rlim_max == RLIM_INFINITY)
        printf("(infinite)\n");
    else
        printf("%10ld\n", limit.rlim_max);
}
