#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/utsname.h>


int main(int argc, char *argv[])
{
	struct utsname name;
	if (uname(&name) != 0) {
		fprintf(stderr, "uname failed.\n");
		return(-1);
	}
	printf("sysname: %s\n", name.sysname);
	printf("nodename: %s\n", name.nodename);
	printf("release: %s\n", name.release);
	printf("version: %s\n", name.version);
	printf("machine: %s\n", name.machine);
	
	
	return 0;
}
