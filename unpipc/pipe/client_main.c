/*
 *更新时间: < 修改人[liyunteng] 2014/10/25 23:36:02 >
 */

#include "../unpipc.h"
#include "./comm.h"


#define FIFO1 "/tmp/fifo.1"
#define FIFO2 "/tmp/fifo.2"

int main(int argc, char *argv[])
{
	int readfd, writefd;


	if ((writefd = open(FIFO1, O_WRONLY)) < 0)
		err_sys("can't open %s", FIFO1);
	if ((readfd = open(FIFO2, O_RDONLY)) < 0)
		err_sys("can't open %s", FIFO2);

	client(readfd, writefd);
	
	close(readfd);
	close(writefd);
	return 0;
}




