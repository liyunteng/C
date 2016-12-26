#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <setjmp.h>

static jmp_buf jmp;
static void f1(int, int, int, int);
static void f2(void);
static int globval;

int main(int argc, char *argv[])
{
	int autoval;
	register int regival;
	volatile int volaval;
	static int staval;

	globval = 1;
	autoval = 2;
	regival = 3;
	volaval = 4;
	staval  = 5;

	if (setjmp(jmp) != 0) {
		printf("after jmp:\n");		
		printf("globval: %d, autoval: %d, regival: %d, "
		       "volaval: %d, staval: %d\n ", globval, autoval,
		       regival, volaval, staval);
		return(0);
	}
	printf("globval: %d, autoval: %d, regival: %d, "
	       "volaval: %d, staval: %d\n ", globval, autoval,
	       regival, volaval, staval);

	globval = 91; autoval = 92; regival = 93; volaval = 94; staval = 95;

	f1(autoval, regival, volaval, staval);

	return 0;
}

static void f1(int i, int j, int k, int l)
{
	printf("in f1():\n");
	printf("globval: %d, autoval: %d, regival: %d, "
	       "volaval: %d, staval: %d\n", globval, i, j, k, l);
	
	f2();
}

static void f2(void)
{
	longjmp(jmp, 1);
}


