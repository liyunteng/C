/*******************************************************************************
 * Author : liyunteng
 * Email : li_yunteng@163.com
 * Created Time : 2014-01-13 13:26
 * Filename : test.c
 * Description : 
 * *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *thread(void *arg)
{
	pid_t pid;
	if ((pid = vfork()) < 0) {
		perror("vfork error.");
		return (void *)-1;
	} else if (pid == 0) {
		if (execl("/usr/bin/wget","wget", "-T", "10", "-t", "2", "api.twilio.com", NULL) == -1) {
			perror("execl error.\n");
			return (void *) -1;
		}
	} else {
	
		if (wait(NULL) == -1) {
			perror("fail to wait.\n");
			return (void *)0;
		}
	}
}

int main(int argc, char *argv[])
{
	pthread_t tid;
	int i, ret;

	for (i=0; i<5; i++) {
		if ((ret = pthread_create(&tid, NULL,  thread, NULL)) < 0) {
			fprintf(stderr, "pthread_create error\n");
			exit -1;
		}
		pthread_join(tid, NULL);
	}

	printf("mani exit.\n");
	

}
