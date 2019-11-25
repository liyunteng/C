/*
 * Description: main
 *
 * Copyright (C) 2018 liyunteng
 * Last-Updated: <2018/12/28 06:47:09 liyunteng>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if 0
int ispalindrome(char *begin, char *end)
{
    if (end - begin < 1) {
        return 1;
    }
    while (begin < end) {
        if (*begin != *end) {
            return 0;
        }
        begin++;
        end--;
    }
    return 1;
}

char* longestPalindrome(char* s) {
    char *begin = s, *end = s;
    int longest = 0;
    char *tmp = NULL;
    char *r = NULL;
    if (strlen(s) < 2) {
        return s;
    }
    for (begin=s; *begin != '\0'; begin++) {
        for (end = begin; *end != '\0'; end++) {
            if(ispalindrome(begin,end)) {
                if (end - begin + 1 > longest) {
                    longest = end - begin + 1;
                    tmp = begin;
                }
                /*
                 * printf("find: ");
                 * for (char *p = begin; p <= end; p++) {
                 *     printf("%c", *p);
                 * }
                 * printf("\n");
                 */
            }
        }
    }
    printf("longest: %d\n", longest);
    if (tmp) {
        r = malloc(longest+1);
        strncpy(r,tmp,longest);
        r[longest] = '\0';
    }
    return r;
}
#endif

int
getPalindromeLength(char *s, int left, int right)
{
    int length = strlen(s);
    while (left >= 0 & right < length && s[left] == s[right]) {
        left--;
        right++;
    }

    return right - left - 1;
}

char *
longestPalindrome(char *s)
{
    int length = strlen(s);
    if (length < 2) {
        return s;
    }

    int begin = 0, end = 0;
    int i;
    for (i = 0; i < length; i++) {
        int len1 = getPalindromeLength(s, i, i);
        int len2 = getPalindromeLength(s, i, i + 1);
        int len  = len1 > len2 ? len1 : len2;
        if (len > end - begin) {
            begin = i - (len - 1) / 2;
            end   = i + len / 2;
        }
    }
    char *r = malloc(end - begin + 2);
    strncpy(r, s + begin, end - begin + 1);
    r[end - begin + 1] = '\0';
    return r;
}

int
main(void)
{
    char *p = "";
    printf("%d\n", ispalindrome(p, p));
    printf("%s\n", longestPalindrome(p));
    return 0;
}
