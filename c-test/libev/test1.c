/*******************************************************************************
 * Author : liyunteng
 * Email : li_yunteng@163.com
 * Created Time : 2014-02-14 12:10
 * Filename : test1.c
 * Description :
 * *****************************************************************************/
#include <ev.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct test_ev {
    ev_io ev;
    int   fd;
};

struct test_ev t_ev;
ev_io          stdin_watcher;
ev_timer       timeout_watcher;
ev_signal      signal_watcher;
ev_child       cw;
ev_stat        file;

static void
file_cb(struct ev_loop *loop, ev_stat *w, int revents)
{
    if (w->attr.st_nlink) {
        printf("test current size %ld\n", (long)w->attr.st_size);
        printf("test current atime %ld\n", (long)w->attr.st_atime);
        printf("test current mtime %ld\n", (long)w->attr.st_mtime);
    } else {

        puts("wow, /root/libev/test is not there, expect problems."
             "if this is windows, they already arrived\n");
    }
}

static void
child_cb(struct ev_loop *loop, ev_child *cw, int revents)
{
    ev_child_stop(loop, cw);
    printf("process %d exited with status %x\n", cw->rpid, cw->rstatus);
}

static void
sigint_cb(struct ev_loop *loop, ev_signal *w, int revents)
{
    puts("catch SIGINT\n");
    ev_break(loop, EVBREAK_ALL);
}

static void
stdin_cb(EV_P_ ev_io *w, int revents)
{
    struct test_ev *t = (struct test_ev *)w;
    printf("ev_io: %p test_io:%p\n", w, t_ev);
    printf("t->fd: %d t->ev.fd:%d t->ev.events:%d\n", t->fd, t->ev.fd, t->ev.events);
    printf("ev.fd : %d, ev.events: %d\n", w->fd, w->events);
    printf("ev_io: %p test_io:%p\n", w, t_ev);
    ev_io_stop(EV_A_ w);
    // ev_break(EV_A_ EVBREAK_ONE);
}

static void
timeout_cb(EV_P_ ev_timer *w, int revents)
{
    puts("timeout");
    // ev_break(EV_A_ EVBREAK_ALL);
}

int
main(int argc, char *argv[])
{
    pid_t           pid;
    struct ev_loop *loop = EV_DEFAULT;

    if ((pid = fork()) < 0) {
        fprintf(stderr, "fork error.\n");
    } else if (pid == 0) {
        printf("children processing\n");
        exit(1);
    } else {
        ev_child_init(&cw, child_cb, pid, 0);
        ev_child_start(loop, &cw);
    }
    t_ev.fd = 999;
    ev_io_init(&t_ev.ev, stdin_cb, 0, EV_READ);
    ev_io_start(loop, &t_ev.ev);

    ev_timer_init(&timeout_watcher, timeout_cb, 0., 3.);
    ev_timer_again(loop, &timeout_watcher);
    ev_timer_start(loop, &timeout_watcher);

    ev_signal_init(&signal_watcher, sigint_cb, SIGINT);
    ev_signal_start(loop, &signal_watcher);

    ev_stat_init(&file, file_cb, "test", 0.);
    ev_stat_start(loop, &file);

    ev_run(loop, 0);

    return 0;
}

/* Local Variables: */
/* compile-command: "clang -Wall -o test1 test1.c -g -lev" */
/* End: */
