/*
 * Description: main
 *
 * Copyright (C) 2018 liyunteng
 * Last-Updated: <2018/12/19 16:38:03 liyunteng>
 */

#include <stdlib.h>
#include <stdio.h>

double findMedianSortedArray(int *nums1, int nums1Size, int *nums2, int nums2Size)
{
    int *m = (int *)malloc(sizeof(int) *(nums1Size + nums2Size));
    int i = 0, j = 0;
    int x = 0;
    int total = nums1Size + nums2Size;
    while (i < nums1Size && j < nums2Size) {
        m[x++] = nums1[i] > nums2[j] ? nums2[j++] : nums1[i++];
    }

    while (i < nums1Size) {
        m[x++] = nums1[i++];
    }
    while (j < nums2Size) {
        m[x++] = nums2[j++];
    }
    if (total % 2 == 0) {
        return ((double)(m[total/2-1] + m[total/2])/2);
    } else {
        return (double)m[total/2];
    }
}

int main(void)
{
    int a[2] = {1, 2};
    int b[2] = {3, 4};
    printf("%f\n", findMedianSortedArray(a , 2, b, 2));
    return 0;
}
