#include <stdio.h>
#include <ev.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define UNIX_DOMAIN "./test.domain"

struct mon_io {
	ev_io io;
	int sockfd;
};

struct mon_io mon_io;
static i=0;

static void sock_cb(struct ev_loop *loop, ev_io *w, int r)
{
	struct mon_io *mi = (struct mon_io *)w;
	char buff[1024];
	int n;

	if ((n=read(mi->sockfd, buff, sizeof(buff)-1)) <= 0) {
	
	}
	buff[n] = '\0';
	i++;
	printf("=========info==========\n");
	printf("%d : %s\n", i, buff);
}

int main(int argc, char *argv[])
{
	socklen_t addr_len;
	int listen_fd;
	int com_fd;
	int ret;
	int i;
	static char recv_buf[1024];
	int len, num;
	struct sockaddr_un ctl_addr;
	struct sockaddr_un srv_addr;
	struct ev_loop *loop = EV_DEFAULT;

	

	listen_fd = socket(PF_UNIX, SOCK_DGRAM, 0);
	if (listen_fd < 0) {
		perror("can't create communication socket");
		return -1;
	}


	srv_addr.sun_family = AF_UNIX;
	strncpy(srv_addr.sun_path, UNIX_DOMAIN, sizeof(srv_addr.sun_path)-1);
	unlink(UNIX_DOMAIN);

	ret = bind(listen_fd, (struct sockaddr *)&srv_addr, sizeof(srv_addr));
	if (ret == -1) {
		perror("bind server socket");
		close(listen_fd);
		return -1;
	}

	mon_io.sockfd = listen_fd;
	ev_io_init(&mon_io.io, sock_cb, mon_io.sockfd, EV_READ);
	ev_io_start(loop, &mon_io.io);
	ev_run(loop, 0);

	unlink(UNIX_DOMAIN);
	return 0;
}
