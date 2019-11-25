#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

extern int fd_map(int out_fd, int argc, char *argv[]);

int
main(int argc, char *argv[])
{
    argc--;
    argv++;

    fd_map(1, argc, argv);

    return 0;
}
