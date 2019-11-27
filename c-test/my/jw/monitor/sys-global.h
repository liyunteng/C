#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

#ifndef __SYS_GLOBAL_H__
#    define __SYS_GLOBAL_H__

#    define SYSMON_CONF "/opt/jw-conf/system/sysmon-conf.xml"
#    define SYSMON_ADDR "./sys-mon-socket-do-not-remove"

#    define _LOG_OPT (LOG_CONS)
#    define log_init() openlog("sys-mon", _LOG_OPT, LOG_DAEMON)
#    define log_release() closelog()

typedef struct _sys_global sys_global_t;
struct _sys_global {
    bool tmpfs;
    struct {
        int info, warning, error;
    } msg_buff_size;
    int power_cnt;
};

#    define info_size msg_buff_size.info
#    define warning_size msg_buff_size.warning
#    define error_size msg_buff_size.error

extern sys_global_t gconf;

void sys_global_init(void);
void sys_mon_conf_check(void);
void sys_mon_load_conf(void);

int tmpfs_msg_count(const char *level);
const char *tmpfs_msg_insert(const char *level, const char *msg);
const char *tmpfs_msg_remove_oldest(const char *level);
ssize_t tmpfs_msg_sorted_link(const char *file);
ssize_t tmpfs_msg_sorted_unlink(const char *file);

void tmpfs_write_alarm(const char *fname, const char *msg);

void dump_sys_global(void);

#endif
