/*
 * instrument-functions.c - instrument-functions
 *
 * Date   : 2020/09/06
 */

/* echo 0 | sudo tee /proc/sys/kernel/randomize_va_space to disable ASLR */
#include <stdio.h>

#define DUMP(func, call) printf("%s:%d func = %p, called = %p\n", __FUNCTION__, __LINE__, func, call)

void __attribute__((__no_instrument_function__))
__cyg_profile_func_enter(void *this_fn, void *call_site)
{
    DUMP(this_fn, call_site);
}

void __attribute__((__no_instrument_function__))
__cyg_profile_func_exit(void *this_fn, void *call_site)
{
    DUMP(this_fn, call_site);
}

void test2()
{
    printf("here\n");
}
void test1()
{
    test2();
}
void test()
{
    test1();
}

int main(void)
{
    test();
    return 0;
}

/* Local Variables: */
/* compile-command: "gcc -Wall -o instrument-functions instrument-functions.c -g -finstrument-functions" */
/* End: */
