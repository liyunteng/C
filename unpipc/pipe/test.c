/*******************************************************************************
 * Author : liyunteng
 * Email : li_yunteng@163.com
 * Created Time : 2014-10-25 16:33
 * Filename : test.c
 * Description :
 * *****************************************************************************/
#include "../unpipc.h"

int
main(int argc, const char *argv[])
{
    pid_t pid;
    int   fd[2];
    char  buf[1024];
    int   n = 0;

    if (pipe(fd) < 0) {
        fprintf(stderr, "pipe failed: %s\n", strerror(errno));
        return errno;
    }

    if ((pid = fork()) < 0) {
        fprintf(stderr, "fork failed: %s\n", strerror(errno));
        return errno;
    } else if (pid == 0) {

        close(fd[1]);
        while ((n = read(fd[0], buf, MAXLINE - 1)) > 0) {

            buf[n] = '\0';
            fprintf(stdout, "%s", buf);
        }
        if (n < 0) {
            fprintf(stderr, "son read failed : %s\n", strerror(errno));
        }
    }
    close(fd[0]);
    while ((n = read(STDIN_FILENO, buf, MAXLINE - 1)) > 0) {

        buf[n] = '\0';
        if (write(fd[1], buf, strlen(buf)) != strlen(buf)) {
            fprintf(stderr, "write failed: %s\n", strerror(errno));
        }
    }
    if (n < 0) {
        fprintf(stderr, "father read failed : %s\n", strerror(errno));
    }

    waitpid(pid, NULL, 0);

    return 0;
}
