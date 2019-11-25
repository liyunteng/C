#include "clog.h"
#include "us_ev.h"
#include "us_mon.h"
#include "us_prewarn.h"
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

extern void us_mod_init(void);
extern void us_mod_release(void);

static void
sig_child(int sig)
{
    int sts;

    while (waitpid(-1, &sts, WNOHANG) > 0)
        ;
}

#ifdef DEBUG
static ev_io dbg_io;
static void
dbg_cb(EV_P_ ev_io *w, int revents)
{
    ev_io_stop(us_main_loop, &dbg_io);
    ev_break(EV_A_ EVBREAK_ALL);
}
#else
static void
dbg_init(void)
{
}
#endif

int
main(int argc, char *argv[])
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHILD, sig_child);
    clog_init();
    us_mod_init();
    dbg_init();
    us_mon_enum_dev();
    clog(LOG_INFO, "Monitor start\n");
    us_loop();
    puts("break!------------");
    us_mod_release();
    clog_release();

    return 0;
}
