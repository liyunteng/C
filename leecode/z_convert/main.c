/*
 * Description: main
 *
 * Copyright (C) 2018 liyunteng
 * Last-Updated: <2018/12/28 04:35:38 liyunteng>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* 3
 * 0   4   8    12
 * 1 3 5 7 9 11 13 15
 * 2   6   10   14
 */
/* 4
 * 0     6       12
 * 1   5 7    11 13
 * 2 4   8 10    14
 * 3     9       15
 */
char *
convert(char *s, int numRows)
{
    int i, j;
    int length = strlen(s);
    if (numRows <= 1) {
        return s;
    }
    char *r     = malloc(length + 1);
    int   base  = numRows * 2 - 2;
    int   left  = 0;
    int   right = 0;
    int   index = 0;
    for (i = 0; i < numRows; i++) {
        left  = base - i * 2;
        right = i * 2;
        for (j = i; j < length;) {
            r[index++] = s[j];
            /*
             * printf("%c[%d]", s[j], j);
             */
            if (left > 0) {
                j += left;
                if (j < length && left < base) {
                    r[index++] = s[j];
                    /*
                     * printf("%c[%d]", s[j], j);
                     */
                }
            }

            if (right > 0) {
                j += right;
            }
        }
    }
    r[index] = '\0';
    return r;
}

int
main(void)
{
    /*
     * char *s = "LEETCODEISHIRING";
     */
    char *s = "Apalindromeisaword,phrase,number,"
              "orothersequenceofunitsthatcanbereadthesamewayineitherdirection,"
              "withgeneralallowancesforadjustmentstopunctuationandworddividers.";
    printf("%s\n", convert(s, 2));
    return 0;
}
