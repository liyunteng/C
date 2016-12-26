#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


static void my_exit(void)
{
	printf("exit!\n");
}

int main(int argc, char *argv[])
{
	char *p = NULL;
	//char *n = "TEST=lyt";
	if (atexit(my_exit) != 0 ){
		fprintf(stderr, "atexit register failed!\n");
		return -1;
	} 

	p = getenv("PWD");
	printf("PWD=%s\n", p);
	/* if (putenv(n) != 0) { */
	/* 	fprintf(stderr, "putenv failed!\n"); */
	/* } */	
	/* p = getenv("TEST"); */
	/* printf("TEST=%s\n", p); */
	if(setenv("TEST", "this is a test", 0) != 0){
		fprintf(stderr, "setenv failed!\n");
	}
	p = getenv("TEST");
	printf("TEST=%s\n", p);

	
	printf("started!\n");

	abort();
	printf("end!\n");
	sleep(10);
	return 0;
}
