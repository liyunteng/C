#include "list.h"
#include <stdio.h>
#include <stdlib.h>

struct test {
    struct list list;
    int         count;
};
struct test root;
int
main(int argc, char *argv[])
{
    struct list *ptr;
    struct list *n;
    struct test *tp;
    int          i;

    init_list(&root.list);
    for (i = 0; i < 10; i++) {
        tp = (struct test *)malloc(sizeof(struct test));
        if (!tp) {
            fprintf(stderr, "mallo error\n");
            return -1;
        }

        tp->count = i;
        list_add(&tp->list, &root.list);
    }

    list_for_each_safe(ptr, n, &root.list)
    {
        tp = list_entry(ptr, struct test, list);
        printf("%d\n", tp->count);
        free(tp);
    }
    return 0;
}
