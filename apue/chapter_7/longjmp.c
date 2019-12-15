/*
 * 7-4.c - setjmp longjmp
 *
 * Author : liyunteng <li_yunteng@163.com>
 * Date   : 2019/06/19
 */
#include "ourhdr.h"
#include <setjmp.h>

#define TOK_ADD 5
jmp_buf jmpbuffer;
void do_line(char *);
void do_jump(void);
int get_token(void);

int
main(void)
{
    char line[MAXLINE];

    if (setjmp(jmpbuffer) != 0) {
        printf("after longjup\n");
    }
    while (fgets(line, MAXLINE, stdin) != NULL) {
        do_line(line);
    }
    return 0;
}
char *tok_ptr;

void
do_line(char *ptr)
{
    int cmd;

    tok_ptr = ptr;
    cmd = get_token();
    printf("# %d\n", cmd);
    switch (cmd) {
    case TOK_ADD:
        do_jump();
        break;
    default:
        break;
    }
    return;
}

void
do_jump(void)
{
    longjmp(jmpbuffer, 1);
}

int
get_token(void)
{
    /* TODO: */
    static int val = 1;
    return val++;
}
