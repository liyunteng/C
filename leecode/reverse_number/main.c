/*
 * Description: main
 *
 * Copyright (C) 2018 liyunteng
 * Last-Updated: <2018/12/28 07:00:42 liyunteng>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

int reverse(int x) {
    char c;
    char *tmp = malloc(32);
    int flag = 0;
    if (x < 0) {
        flag = 1;
    }
    int len = snprintf(tmp, 32, "%d", abs(x));
    int i;
    for (i=0; i < len/2; i++) {
        c = tmp[i];
        tmp[i] = tmp[len-i-1];
        tmp[len-i-1] = c;
    }
    printf("%s\n", tmp);
    long r = atol(tmp);
    if (r > INT_MAX) {
        r = 0;
    }
    if (flag) {
        r = 0 - r;
    }
    return r;
}

int main(void)
{
    printf("%d\n", reverse(-123));
    return 0;
}
