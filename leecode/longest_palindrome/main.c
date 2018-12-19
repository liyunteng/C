/*
 * Description: main
 *
 * Copyright (C) 2018 liyunteng
 * Last-Updated: <2018/12/19 17:27:50 liyunteng>
 */
#include <stdlib.h>
#include <stdio.h>

char *longestPalindrome(char *s)
{
    char *p = NULL;
    char *pr = NULL;
    int i = 0;
    int length = 0, longest = 0;
    for (p = s; *p != '\0'; p++) {
        length = 0;
        for (i = 0; p-i >= s && *(p+i) != '\0'; i++) {
            if (p[i] == p[-i]) {
                length ++;
            } else {
                break;
            }
        }
        if (length > 0 && length > longest) {
            longest = length;
            pr = p - length;
        }
    }
    char *res = malloc(longest*2 + 2);
    for (i = 0; i < longest*2 + 1; i++) {
        res[i] = pr[i];
    }
    return res;
}

int main(void)
{
    char *p = "babad";
    printf("%s\n", longestPalindrome(p));
    return 0;
}
