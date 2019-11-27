#include <linux/kdb.h>
#include <linux/module.h>

static int
kdb_hello_cmd(int argc, const char **argv)
{
    if (argc > 1)
        return -1;

    if (argc)
        kdb_printf("hello %s.\n", argv[1]);
    else
        kdb_printf("hello world.\n");

    return 0;
}

static int __init
kdb_hello_cmd_init(void)
{
    kdb_register("hello", kdb_hello_cmd, "[string]",
                 "Say Hello World or Hello [string]", 0);
    return 0;
}

static void __exit
kdb_hello_cmd_exit(void)
{
    kdb_unregister("hello");
}

module_init(kdb_hello_cmd_init);
module_exit(kdb_hello_cmd_exit);

MODULE_AUTHOR("WindRiver");
MODULE_DESCRIPTION("KDB example to add a hello command");
MODULE_LICENSE("GPL");
