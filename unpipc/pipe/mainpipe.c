/*
 *更新时间: < 修改人[liyunteng] 2014/10/26 11:11:07 >
 */
#include "../unpipc.h"
#include "./comm.h"

int
main(int argc, char *argv[])
{
    int   pipe1[2], pipe2[2];
    pid_t childpid;

    if (pipe(pipe1) < 0) {
        fprintf(stderr, "pipe failed: %s\n", strerror(errno));
    }
    if (pipe(pipe2) < 0) {
        fprintf(stderr, "pipe failed: %s\n", strerror(errno));
    }

    if ((childpid = fork()) < 0) {
        fprintf(stderr, "fork failed: %s\n", strerror(errno));
        return errno;
    } else if (childpid == 0) {
        close(pipe1[1]);
        close(pipe2[0]);

        server(pipe1[0], pipe2[1]);
        return 0;
    }

    close(pipe1[0]);
    close(pipe2[1]);

    client(pipe2[0], pipe1[1]);

    waitpid(childpid, NULL, 0);

    return 0;
}
