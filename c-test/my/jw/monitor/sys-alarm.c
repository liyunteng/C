/*******************************************************************************
 * Author : liyunteng
 * Email : li_yunteng@163.com
 * Created Time : 2014-02-19 18:21
 * Filename : sys-alarm.c
 * Description :
 * *****************************************************************************/
#include "../pic_ctl/pic_ctl.h"
#include "sys-action.h"
#include "sys-event.h"
#include "sys-utils.h"
#include <regex.h>
#include <stdio.h>
#include <strings.h>
#include <sys/types.h>

#define SHELL "/bin/sh"
#define BUZZER_ON_CMD "sys-manager system --set buzzer --value inc >/dev/null"
#define BUZZER_OFF_CMD "sys-manager system --set buzzer --value dec >/dev/null"

struct event_record {
    char param[128];
    struct list list_entry;
};

int
_safe_system(const char *cmd)
{
    pid_t pid;

    pid = fork();
    if (pid < 0)
        return -1;
    if (pid == 0) {
        execl(SHELL, "sh", "-c", cmd, NULL);
        exit(127);
    }

    return 0;
}

int
_gconf_level_count(const char *level)
{
    if (!level)
        return 0;

    if (!strcmp(level, "info"))
        return gconf.info_size;
    else if (!strcmp(level, "warning"))
        return gconf.warning_size;
    else if (!strcmp(level, "error"))
        return gconf.error_size;

    return 0;
}

void
sys_alarm_default(void *event)
{
    sys_event_t *ev = (sys_event_t *)event;

    if (!gconf.tmpfs)
        return;

    if (strlen(ev->msg) == 0)
        return;

    if (tmpfs_msg_count(ev->level) > _gconf_level_count(ev->level))
        tmpfs_msg_sorted_unlink(tmpfs_msg_remove_oldest(ev->level));

    tmpfs_msg_sorted_link(tmpfs_msg_insert(ev->level, ev->msg));
}

static LIST_INIT(buzzer_list);

void
sys_alarm_buzzer_on(void *event)
{
    struct list *n, *nt;
    struct event_record *er;
    sys_event_t *ev = (sys_event_t *)event;

    list_iterate_safe(n, nt, &buzzer_list)
    {
        er = list_struct_base(n, struct event_record, list_entry);

        if (strcmp(er->param, ev->param) == 0)
            return;
    }

    er = malloc(sizeof(*er));
    strcpy(er->param, ev->param);
    list_add(&buzzer_list, &er->list_entry);

    _safe_system(BUZZER_ON_CMD);
}

void
sys_alarm_buzzer_off(void *event)
{
    struct list *n, *nt;
    struct event_record *er;
    sys_event_t *ev = (sys_event_t *)event;

    list_iterate_safe(n, nt, &buzzer_list)
    {
        er = list_struct_base(n, struct event_record, list_entry);
        if (strcmp(er->param, ev->param) == 0) {
            list_del(&er->list_entry);
            free(er);

            _safe_system(BUZZER_OFF_CMD);
        }
    }
}

void
sys_alarm_notify_tmpfs(void *event)
{
    sys_event_t *ev = (sys_event_t *)event;

    if (strlen(ev->msg) == 0)
        return;

    if (ev)
        tmpfs_write_alarm(ev->param, ev->msg);
}

char *
_get_next_disk_slot(const char *str)
{
    int cflags = REG_EXTENDED;
    int status;
    regmatch_t pmatch[1];
    const size_t nmatch = 1;
    regex_t reg;
    const char *pattern = "[0-9]:[0-9]+$";
    static char buf[128];
    static char slot[12];

    if (buf[0] == '\0') {
        if (strncmp(str, "disk", 4))
            return NULL;
        strcpy(buf, str);
    }

    bzero(slot, 12);
    regcomp(&reg, pattern, cflags);
    status = regexec(&reg, buf, nmatch, pmatch, 0);
    if (status == 0) {
        int i, j;
        for (j = 0, i = pmatch[0].rm_so; i < pmatch[0].rm_eo; i++, j++) {
            slot[j] = buf[i];
        }

        buf[pmatch[0].rm_so - 1] = '\0';
        slot[j]                  = '\0';
    }

    regfree(&reg);

    if (slot[0] == '\0') {
        buf[0] = '\0';
        return NULL;
    }
    return slot;
}

void
sys_alarm_diskled_on(void *event)
{
    sys_event_t *ev = (sys_event_t *)event;

    char *p;
    while ((p = _get_next_disk_slot(ev->param)) != NULL) {
        int enc, slot;
        sscanf(p, "%d:%d", &enc, &slot);
        pic_set_led(slot - 1, PIC_LED_ON, 0);
    }
}

void
sys_alarm_diskled_off(void *event)
{
    sys_event_t *ev = (sys_event_t *)event;

    char *p;
    while ((p = _get_next_disk_slot(ev->param)) != NULL) {
        int enc, slot;
        sscanf(p, "%d:%d", &enc, &slot);
        pic_set_led(slot - 1, PIC_LED_OFF, 0);
    }
}

void
sys_alarm_diskled_blink1s1(void *event)
{
    sys_event_t *ev = (sys_event_t *)event;

    char *p;
    while ((p = _get_next_disk_slot(ev->param)) != NULL) {
        int enc, slot;
        sscanf(p, "%d:%d", &enc, &slot);
        pic_set_led(slot - 1, PIC_LED_BLINK, PIC_LED_FREQ_NORMAL);
    }
}

void
sys_alarm_diskled_blink1s4(void *event)
{
    sys_event_t *ev = (sys_event_t *)event;

    char *p;
    while ((p = _get_next_disk_slot(ev->param)) != NULL) {
        int enc = -1, slot = -1;
        sscanf(p, "%d:%d", &enc, &slot);
        pic_set_led(slot - 1, PIC_LED_BLINK, PIC_LED_FREQ_FAST);
    }
}

void
sys_alarm_diskled_blink2s1(void *event)
{
    sys_event_t *ev = (sys_event_t *)event;

    char *p;
    while ((p = _get_next_disk_slot(ev->param)) != NULL) {
        int enc = -1, slot = -1;
        sscanf(p, "%d:%d", &enc, &slot);
        pic_set_led(slot - 1, PIC_LED_BLINK, PIC_LED_FREQ_SLOW);
    }
}

static LIST_INIT(sysled_list);
static int sysled_cnt = 0;

void
sys_alarm_sysled_on(void *event)
{
    struct list *n, *nt;
    struct event_record *er;
    sys_event_t *ev = (sys_event_t *)event;

    list_iterate_safe(n, nt, &sysled_list)
    {
        er = list_struct_base(n, struct event_record, list_entry);
        if (strcmp(er->param, ev->param) == 0)
            return;
    }

    er = malloc(sizeof(*er));
    strcpy(er->param, ev->param);
    list_add(&sysled_list, &er->list_entry);

    if (sysled_cnt == 0)
        sb_gpio28_set(true);
    sysled_cnt++;

#ifdef _DEBUG
    printf("sysled_cnt: %d\n", sysled_cnt);
#endif
}

void
sys_alarm_sysled_off(void *event)
{
    struct list *n, *nt;
    struct event_record *er;
    sys_event_t *ev = (sys_event_t *)event;

    list_iterate_safe(n, nt, &sysled_list)
    {
        er = list_struct_base(n, struct event_record, list_entry);
        if (strcmp(er->param, ev->param) == 0) {
            list_del(&er->list_entry);
            free(er);

            if (sysled_cnt > 0)
                sysled_cnt--;
            if (sysled_cnt == 0)
                sb_gpio28_set(false);
#ifdef _DEBUG
            printf("sysled_cnt: %d\n", sysled_cnt);
#endif
        }
    }
}

struct _handler_map {
    char name[128];
    sys_alarm_handler handler;
};

struct _handler_map _map[] = {

    {"default", sys_alarm_default},
    {"buzzer-on", sys_alarm_buzzer_on},
    {"buzzer-off", sys_alarm_buzzer_off},
    {"disk-led-on", sys_alarm_diskled_on},
    {"disk-led-off", sys_alarm_diskled_off},
    {"disk-led-blink1s4", sys_alarm_diskled_blink1s4},
    {"disk-led-blink1s1", sys_alarm_diskled_blink1s1},
    {"disk-led-blink2s1", sys_alarm_diskled_blink2s1},
    {"sys-led-on", sys_alarm_sysled_on},
    {"sys-led-off", sys_alarm_sysled_off},
    {"notify-tmpfs", sys_alarm_notify_tmpfs},
    {"\0", NULL}};

void
sys_alarm_set_handler(sys_alarm_t *alarm, const char *handler_name)
{
    struct _handler_map *p = &_map[0];

    if (alarm && handler_name) {
        while (p->name[0] != '\0') {
            if (!strcmp(handler_name, p->name)) {
                alarm->handler = p->handler;
                return;
            }
            p++;
        }
    }
}
