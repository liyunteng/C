/*******************************************************************************
* Author : liyunteng
* Email : li_yunteng@163.com
* Created Time : 2014-02-20 09:29
* Filename : sys-event.c
* Description : 
* *****************************************************************************/
#include <stdio.h>
#include "sys-event.h"

void sys_event_zero(sys_event_t * ev)
{
    if (ev)
	ev->module[0] = ev->event[0] = ev->param[0] = ev->msg[0] = '\0';
}

sys_event_conf_t *sys_event_conf_alloc()
{
    sys_event_conf_t *ec;

    if ((ec = (sys_event_conf_t *) malloc(sizeof(*ec))) != NULL) {
	list_init(&ec->event_list);
	ec->count = 0;
	ec->action = NULL;
	return ec;
    }

    syslog(LOG_NOTICE, "sys_event_conf_alloc(): fail");
    return NULL;
}


void sys_event_fill(sys_event_t * ev, const char *key, const char *value)
{
    if (!strcmp(key, "module"))
	strcpy(ev->module, value);
    else if (!strcmp(key, "event"))
	strcpy(ev->event, value);
    else if (!strcmp(key, "param"))
	strcpy(ev->param, value);
    else if (!strcmp(key, "msg"))
	strcpy(ev->msg, value);
}
