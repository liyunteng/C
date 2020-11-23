/*
 * 5-4.c - create tmp file
 *
 * Author : liyunteng <li_yunteng@163.com>
 * Date   : 2019/06/18
 */
#include "ourhdr.h"

int
main(void)
{
    char name[L_tmpnam], line[MAXLINE];
    FILE *fp;

#ifdef __GNU__
    printf("%s\n", tmpnam_r(NULL));

    tmpnam_r(name);
    printf("%s\n", name);
#endif

    if ((fp = tmpfile()) == NULL) {
        err_sys("tmpfile error");
    }

    fputs("one line of output\n", fp);
    rewind(fp);
    if (fgets(line, sizeof(line), fp) == NULL) {
        err_sys("fgets error");
    }
    fputs(line, stdout);

    return 0;
}
