/*
 * Description: 3-1 seek stdin
 *
 * Copyright (C) 2019 liyunteng
 * Last-Updated: <2019/06/12 00:42:55 liyunteng>
 */
#include <sys/types.h>
#include "ourhdr.h"

int main(void)
{
    if (lseek(STDIN_FILENO, 0, SEEK_CUR) == -1) {
        printf("cannot seek\n");
    } else {
        printf("seek OK\n");
    }
    return 0;
}
