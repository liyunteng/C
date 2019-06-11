/*
 * Description: 2-3
 *
 * Copyright (C) 2019 liyunteng
 * Last-Updated: <2019/06/11 23:40:32 liyunteng>
 */
#include <errno.h>
#include <limits.h>
#include "ourhdr.h"

#ifdef OPEN_MAX
static int openmax = OPEN_MAX;
#else
static int openmax = 0;
#endif // OPEN_MAX

#define OPEN_MAX_GUESS 256

int open_max(void)
{
    if (openmax == 0) {
        errno = 0;
        if ((openmax = sysconf(_SC_OPEN_MAX)) < 0) {
            if (errno == 0)
                openmax = OPEN_MAX_GUESS;
            else
                err_sys("sysconf error for _SC_OPEN_MAX");
        }
    }
    return (openmax);
}

/* for test */
int main(void)
{
    printf("open max: %d\n", open_max());
    return 0;
}
