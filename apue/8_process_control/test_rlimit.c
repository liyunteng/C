#include <stdio.h>
#include <sys/resource.h>

int main(int argc, char *argv[])
{
	struct rlimit rlimit;


	if (getrlimit(RLIMIT_FSIZE, &rlimit) != 0) {
		fprintf(stderr, "getrlimit failed.\n");
		return -1;
	}

	printf("limit: %d\n", (int)rlimit.rlim_cur);
	return 0;
}
