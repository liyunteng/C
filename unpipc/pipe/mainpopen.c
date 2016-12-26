/*
 *更新时间: < 修改人[liyunteng] 2014/10/26 10:39:41 >
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include <string.h>

#define MAXLINE 1024

int main(int argc, char *argv[])
{

	size_t		n;
	char		buf[MAXLINE], command[MAXLINE];
	FILE		*fp;

	if (fgets(buf, MAXLINE, stdin) < 0) {
		fprintf(stderr, "fgets failed: %s\n", strerror(errno));
	}
	n = strlen(buf);
	if (buf[n-1] == '\n')
		n--;

	snprintf(command, sizeof(command), "cat %s", buf);
	if ((fp = popen(command, "r")) == NULL) {
		fprintf(stderr, "popen failed: %s\n", strerror(errno));
	}
	while (fgets(buf, MAXLINE, fp) != NULL) {

		fputs(buf, stdout);
	}
	pclose(fp);
	return 0;
}






