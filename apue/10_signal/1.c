#include <stdio.h>
#include <pwd.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
/* getpwnam 是不可重入函数， 在signal中会出现异常  */
static void my_alarm(int signo)
{
    struct passwd *rootptr;

    printf("in signal handler\n");
    if ((rootptr = getpwnam("lyt")) == NULL) {
	fprintf(stderr, "signal getpwname failed!\n");
	exit(-1);
    }
    printf("signal: pw = %s name = %s shell = %s\n", rootptr->pw_passwd,
	   rootptr->pw_name, rootptr->pw_shell);
    alarm(1);

}

int main(int argc, char *argv[])
{
    struct passwd *ptr;
    signal(SIGALRM, my_alarm);
    alarm(1);

    for (;;) {
	if ((ptr = getpwnam("lyt")) == NULL) {
	    fprintf(stderr, "main getpwname failed!\n");
	    exit(-1);
	}
	if (strcmp(ptr->pw_name, "lyt") != 0) {
	    printf("return value corrupted!, pw_name = %s\n",
		   ptr->pw_name);
	}
	printf("main : pw = %s name = %s \n", ptr->pw_passwd,
	       ptr->pw_name);
    }


    return 0;
}
