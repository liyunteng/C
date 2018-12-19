/*
 * Description: main
 *
 * Copyright (C) 2018 liyunteng
 * Last-Updated: <2018/12/19 15:56:30 liyunteng>
 */

#include <stdlib.h>
#include <stdio.h>

int lengthOfLongestSubString(char *s) {
    int r = 0;
    int length = 0;
    for (char *p = s; *p != '\0'; p++, length++) {}
    int map[256] = {0};
    int i, j;
    for (i = 0, j = 0; j < length; j++) {
        i = map[(int)s[j]] > i ? map[(int)s[j]] : i;
        r = r > j - i + 1 ? r : j - i + 1;
        map[(int)s[j]] = j + 1;
    }
    return r;
}


int main(void)
{
    char *p = "alqebriavxoo";
    printf("%d\n", lengthOfLongestSubString(p));
    return 0;
}
