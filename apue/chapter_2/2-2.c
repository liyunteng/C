/*
 * Description: 2-2 dynamic alloc for path
 *
 * Copyright (C) 2019 liyunteng
 * Last-Updated: <2019/06/11 23:41:22 liyunteng>
 */
#include "ourhdr.h"
#include <errno.h>
#include <limits.h>

#ifdef PATH_MAX
static int pathmax = PATH_MAX;
#else
static int pathmax = 0;
#endif  // PATH_MAX

#define PATH_MAX_GUESS 1024

char *
path_alloc(int *size)
{
    char *ptr;
    if (pathmax == 0) {
        errno = 0;
        if ((pathmax = pathconf("/", _PC_PATH_MAX)) < 0) {
            if (errno == 0) {
                pathmax = PATH_MAX_GUESS;
            } else {
                err_sys("pathconf error for _PC_PATH_MAX");
            }
        } else
            pathmax++;
    }

    if ((ptr = malloc(pathmax + 1)) == NULL)
        err_sys("malloc error for pathname");

    if (size != NULL)
        *size = pathmax + 1;

    return (ptr);
}

/* for test */
int
main(void)
{
    char *p  = NULL;
    int size = 0;
    p        = path_alloc(&size);
    printf("pathmax: %d\n", size);
    return 0;
}
