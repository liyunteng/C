/*
 * Description: main
 *
 * Copyright (C) 2018 liyunteng
 * Last-Updated: <2018/12/16 12:11:40 liyunteng>
 */
#include <stdlib.h>
#include <stdio.h>

int *twoSum(int *nums, int numsSize, int target) {
    int *result = NULL;
    int a, b;
    for (a = 0; a < numsSize; a++) {
        for (b = a+1; b < numsSize; b++) {
            if (nums[a] + nums[b] == target) {
                result = malloc(sizeof(int) * 2);
                result[0] = a;
                result[1] = b;
                return result;
            }
        }
    }
    return result;
}

int main(void)
{
    int *test = malloc(sizeof(int) * 100);
    for (int i=0; i < 100; i++) {
        test[i] = i;
    }

    int *r = twoSum(test,100, 98+99);
    if (r != NULL) {
        printf("%d %d", r[0], r[1]);
    }
    return 0;
}
