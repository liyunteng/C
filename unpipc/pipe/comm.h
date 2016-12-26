#ifndef COMM__H__
#define COMM__H__

#include "../unpipc.h"

#define MSGDATA		(PIPE_BUF - 2*sizeof(long))
#define MSGHDRSIZE	(sizeof(struct msg) - MSGDATA)


struct msg{
	long msg_len;
	long msg_type;
	char msg_data[MSGDATA];
};

ssize_t msg_send(int, struct msg*);
ssize_t msg_recv(int, struct msg*);
void client(int readfd, int writefd);
void server(int, int);

#endif // COMM__H__
