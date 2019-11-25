/*******************************************************************************
 * Author : liyunteng
 * Email : li_yunteng@163.com
 * Created Time : 2014-02-14 12:05
 * Filename : sys-mon.c
 * Description :
 * *****************************************************************************/
#include "sys-action.h"
#include "sys-event.h"
#include "sys-interval-check.h"
#include "sys-module.h"
#include <ev.h>
#include <json/json.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

struct mon_io {
    ev_io io;
    int   sockfd;
};

static void            mon_io_cb(EV_P_ ev_io *w, int r);
static int             mon_serv_create();
static struct mon_io   mon_io;
static struct ev_loop *mon_loop = NULL;

void
mon_release(int sig)
{
    signal(sig, SIG_IGN);

    close(mon_io.sockfd);
    ev_io_stop(mon_loop, &mon_io.io);

    sys_module_release();
    sys_action_release();
    log_release();
    signal(sig, SIG_DFL);
}

static void
mon_io_cb(EV_P_ ev_io *w, int r)
{
    ssize_t             n;
    sys_event_t         ev;
    sys_event_conf_t *  ec;
    char                buff[1024];
    struct json_object *obj;

    struct mon_io *mi = (struct mon_io *)w;

    if ((n = read(mi->sockfd, buff, sizeof(buff) - 1)) < 0) {
    }
    buff[n] = '\0';

    syslog(LOG_NOTICE, "ev: get msg from socket!");
    sys_event_zero(&ev);
    obj = json_tokener_parse(buff);
    json_object_object_foreach(obj, key, val)
    {
        sys_event_fill(&ev, key, json_object_get_string(val));
    }

    if ((ec = sys_module_event_get(ev.module, ev.event)) != NULL) {
        sys_module_event_update(ec);
        ev.level = ec->level;
        do_sys_action(ec->action, &ev);
        return;
    }
}

int
mon_serv_create()
{
    struct sockaddr_un localaddr;
    size_t             addr_len;
    int                sockfd;

    if ((sockfd = socket(PF_UNIX, SOCK_DGRAM, 0)) < 0) {
        syslog(LOG_ERR, "fail to create local socket!\n");
        raise(SIGTERM);
        return -1;
    }

    localaddr.sun_family = AF_UNIX;
    strcpy(localaddr.sun_path, SYSMON_ADDR);
    unlink(localaddr.sun_path);
    addr_len = strlen(localaddr.sun_path) + sizeof(localaddr.sun_family);

    if (bind(sockfd, (struct sockaddr *)&localaddr, addr_len) < 0) {
        syslog(LOG_ERR, "faile to bind local socket!\n");
        close(sockfd);
        raise(SIGTERM);
        return -1;
    }

    syslog(LOG_INFO, "create local socket for sys-mon OK!\n");
    return sockfd;
}

void
usage()
{
    fprintf(stderr, "sys-mon [--print-on]\n");
    exit(-1);
}

int global_print_on = 0;

int
main(int argc, char *argv[])
{
    if (argc > 1) {
        if (strcmp(argv[1], "--print-on") == 0)
            global_print_on = 1;
        else
            usage();
    }

    signal(SIGPIPE, SIG_IGN);
    signal(SIGTERM, mon_release);
    signal(SIGINT, mon_release);
    signal(SIGALRM, do_interval_check);

    log_init();
    sys_mon_load_conf();

#ifdef _DEBUG
    dump_module_event();
    dump_action_alarm();
    dump_sys_global();
    dump_self_run();
#endif

    alarm(CHECK_INTVAL);
    mon_io.sockfd = mon_serv_create();

    mon_loop = EV_DEFAULT;
    ev_io_init(&mon_io.io, mon_io_cb, mon_io.sockfd, EV_READ);
    ev_io_start(mon_loop, &mon_io.io);
    ev_run(mon_loop, 0);

    log_release();

    return 0;
}
