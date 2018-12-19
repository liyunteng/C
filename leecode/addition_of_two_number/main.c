/*
 * Description: main
 *
 * Copyright (C) 2018 liyunteng
 * Last-Updated: <2018/12/16 13:07:35 liyunteng>
 */

#include <stdlib.h>
#include <stdio.h>


struct ListNode {
    int val;
    struct ListNode *next;
};

struct ListNode *addTwoNumbers(struct ListNode *l1, struct ListNode *l2) {
    struct ListNode *result = NULL;
    struct ListNode *p1, *p2;
    int flag = 0;
    struct ListNode **pc = &result;
    for (p1 = l1, p2 = l2; p1 || p2 || flag;) {
        struct ListNode *pr = malloc(sizeof(struct ListNode));
        int val = flag;
        flag = 0;
        val = p1 ? val+p1->val : val;
        val = p2 ? val+p2->val : val;
        if(val >= 10) {
            flag = val / 10;
            val = val % 10;
        }
        pr->val = val;
        pr->next = NULL;
        *pc = pr;
        pc = &(pr->next);

        if (p1) {
            p1 = p1->next;
        }
        if (p2) {
            p2 = p2->next;
        }
    }
    return result;
}

int main(void)
{
    int i;
    struct ListNode *p1 = NULL;
    struct ListNode *p2 = NULL;
    struct ListNode **tmp = NULL;
    for (i = 1; i < 8; i++) {
        struct  ListNode *p = malloc(sizeof(struct ListNode));
        p-> val = i;
        for (tmp = &p1; *tmp != NULL; tmp = &(*tmp)->next) {}
        *tmp = p;
    }
    for (i = 1; i < 7; i++) {
        struct  ListNode *p = malloc(sizeof(struct ListNode));
        p-> val = i;
        for (tmp = &p2; *tmp != NULL; tmp = &(*tmp)->next) {}
        *tmp = p;
    }

    for (tmp = &p1; *tmp != NULL; tmp = &(*tmp)->next) {
        printf("%d", (*tmp)->val);
    }
    printf("\n");
    for (tmp = &p2; *tmp != NULL; tmp = &(*tmp)->next) {
        printf("%d", (*tmp)->val);
    }
    printf("\n");
    struct ListNode *result = addTwoNumbers(p1,p2);
    for (tmp = &result; *tmp != NULL; tmp = &(*tmp)->next) {
        printf("%d", (*tmp)->val);
    }
    printf("\n");
    return 0;
}
