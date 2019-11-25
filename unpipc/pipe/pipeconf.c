/*
 *更新时间: < 修改人[liyunteng] 2014/10/26 12:33:04 >
 */

#include "../unpipc.h"

int
main(int argc, char *argv[])
{
    if (argc != 2)
        err_quit("usage: pipeconf <pathname>");
    printf("PIPE_BUF = %ld, OPEN_MAX = %ld\n", pathconf(argv[1], _PC_PIPE_BUF),
           sysconf(_SC_OPEN_MAX));

    return 0;
}
