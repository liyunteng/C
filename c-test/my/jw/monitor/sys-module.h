#include "list.h"
#include "sys-event.h"
#include "sys-global.h"

#ifndef __SYS_MODULE_H__
#    define __SYS_MODULE_H__

typedef struct _sys_module sys_module_t;
struct _sys_module {
    struct list list;
    struct list event_list;
    char name[128];
};

extern struct list _gmodule_list;

void sys_module_init();
void sys_module_release();

bool sys_module_add(const char *name);

sys_module_t *sys_module_get(const char *name);

bool sys_module_event_add(const char *name, sys_event_conf_t *event);

sys_event_conf_t *sys_module_event_get(const char *name, const char *event);

void sys_module_event_update(sys_event_conf_t *ec);

void dump_module_event();

#endif
