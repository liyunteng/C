#include "list.h"
#include "sys-global.h"

#ifndef _SYS_INTERVAL_CHECK__
#    define _SYS_INTERVAL_CHECK__

enum CAPTURE_RESULT {
    VAL_NORMAL,
    VAL_WARNING,
    VAL_ERROR,
    VAL_CRIT,
    VAL_EMERG,
};

#    define isExecutable(item) (item->_capture)
#    define isExpried(item)                                                    \
        (time(NULL) - item->_last_update >= item->check_intval)
#    define update(item) item->_last_update = time(NULL)
#    define execute(item) item->_capture()

#    define CHECK_INTVAL 5

typedef int (*capture_func)(char *msg_out);

bool isCaptureSupported(const char *mod);
capture_func capture_get(const char *mode);

typedef struct _sys_capture_conf sys_capture_t;
struct _sys_capture_conf {
    struct list list;
    char name[64];
    int check_intval;
    int min_thr, max_thr;
    time_t _last_update;
    capture_func _capture;
    int _error;
    bool _preset;
};

extern struct list _g_capture;
extern const char *mod_cap_list[];
extern const char *mod_ch_name[];

void sys_capture_init();
sys_capture_t *sys_capture_alloc();
void sys_capture_set_handler(sys_capture_t *cap);
void sys_capture_add(sys_capture_t *cap);

void do_interval_check(int sig);

void dump_self_run();

#endif
