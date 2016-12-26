/*******************************************************************************
* Author : liyunteng
* Email : li_yunteng@163.com
* Created Time : 2014-08-23 14:59
* Filename : test_vfork.c
* Description : 
* *****************************************************************************/
#include <stdio.h>
#include <unistd.h>

int glob = 5;

int main(int argc, char *argv[])
{
	int var;
	pid_t pid;

	var = 88;
	printf("before vfork");

	if ((pid = vfork()) < 0) {
		fprintf(stderr, "vfork failed!\n");
	} else if (pid == 0) {
		glob ++;
		var ++;
		_exit(0);
	} else {
		glob++;
		var++;
	}

	printf("pid = %d, glob = %d, var = %d\n", getpid(), glob, var);
	
	return 0;
}


