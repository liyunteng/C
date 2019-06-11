/*
 * Description: 1-4 print pid
 *
 * Copyright (C) 2019 liyunteng
 * Last-Updated: <2019/06/11 00:06:14 liyunteng>
 */
#include "ourhdr.h"

int main(void)
{
    printf("hello world from process ID %d\n", getpid());
    return 0;
}

/* Local Variables: */
/* compile-command: "clang -Wall -o 1-4 1-4.c -g -I../include -L../lib -lourhdr" */
/* End: */
