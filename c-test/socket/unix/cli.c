#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#define UNIX_DOMAIN "./test.domain"

int
main(int argc, char *argv[])
{
    int connect_fd;
    int ret;
    char snd_buf[1024];
    int i;
    static struct sockaddr_un srv_addr;

    connect_fd = socket(PF_UNIX, SOCK_DGRAM, 0);
    if (connect_fd < 0) {
        perror("create socket");
        return -1;
    }

    srv_addr.sun_family = AF_UNIX;
    strcpy(srv_addr.sun_path, UNIX_DOMAIN);

    /*
      ret = connect(connect_fd, (struct sockaddr *)&srv_addr, sizeof(srv_addr));
      if (ret == -1) {
      perror("connect");
      close(connect_fd);
      return -1;
      }

      memset(snd_buf, 0, 1024);
      strcpy(snd_buf, "messasge from client");
      for (i=0;i<100; i++)
      write(connect_fd, snd_buf, sizeof(snd_buf));

      close(connect_fd);
    */

    strcpy(snd_buf, "messasge from client");
    while (1) {
        sendto(connect_fd, snd_buf, sizeof(snd_buf), 0,
               (struct sockaddr *)&srv_addr, sizeof(srv_addr));
        sleep(3);
    }
    return 0;
}
