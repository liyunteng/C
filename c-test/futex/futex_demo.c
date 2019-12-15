/*
 * futex_demo.c - futex_demo
 *
 * Date   : 2019/12/15
 */

#include <stdio.h>
#include <errno.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <sys/time.h>


#define errExit(msg)                            \
do {                                            \
    perror(msg);                                \
    exit(EXIT_FAILURE);                         \
} while(0)

static int *futex1, *futex2, *iaddr;

static int futex(int *uaddr, int futex_op, int val,
                 const struct timespec *timeout,
                 int *uaddr2, int val3)
{
    return syscall(SYS_futex, uaddr, futex_op, val,
                   timeout, uaddr2, val3);
}

static void fwait(int *futexp)
{
    int s;

    while(1) {
        const int zero = 0;
        if (atomic_compare_exchange_strong((atomic_int *)futexp, (int *)&zero, 1))
            break;              /* Yes */

        /* WAIT */
        s = futex(futexp, FUTEX_WAIT, 0, NULL, NULL, 0);
        if (s == -1 && errno != EAGAIN)
            errExit("futex-FUTEX_WAIT");
    }
}

static void fpost(int *futexp)
{
    int s;
    const int one = 1;
    if (atomic_compare_exchange_strong((atomic_int *)futexp, (int *)&one, 0)) {
        s = futex(futexp, FUTEX_WAKE, 1, NULL, NULL, 0);
        if (s == -1) {
            errExit("futex-FUTEX_WAK");
        }
    }
}


int main(int argc, char *argv[])
{
    pid_t childPid;
    int j, nloops;

    setbuf(stdout, NULL);

    nloops = (argc > 1) ? atoi(argv[1]) : 5;
    iaddr = mmap(NULL, sizeof(int) * 2, PROT_READ | PROT_WRITE,
                 MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (iaddr == MAP_FAILED)
        errExit("mmap");

    futex1 = &iaddr[0];
    futex2 = &iaddr[1];

    *futex1 = 0;
    *futex2 = 1;

    childPid = fork();
    if (childPid == -1)
        errExit("fork");

    if (childPid == 0) {        /* child */
        for (j = 0; j < nloops; j++) {
            fwait(futex1);
            printf("Child  (%ld) %d\n", (long)getpid(), j);
            fpost(futex2);
        }
        exit(EXIT_SUCCESS);
    }

    /* parent */
    for (j = 0; j < nloops; j++) {
        fwait(futex2);
        printf("Parent (%ld) %d\n", (long)getpid(), j);
        fpost(futex1);
    }


    wait(NULL);
    return 0;
}
