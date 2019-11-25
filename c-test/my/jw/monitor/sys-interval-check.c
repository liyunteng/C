/*******************************************************************************
 * Author : liyunteng
 * Email : li_yunteng@163.com
 * Created Time : 2014-02-19 15:01
 * Filename : sys-interval-check.c
 * Description :
 * *****************************************************************************/
#include "sys-interval-check.h"
#include "../common/log.h"
#include "sys-global.h"
#include "sys-mon.h"
#include <signal.h>
#include <stdio.h>
#include <time.h>

struct list _g_capture;

static const char *
MOD_NAME(const char *mod)
{
    int         i;
    static char _not_found[2] = "";

    for (i = 0; mod_cap_list[i]; i++) {
        if (!strcmp(mod_cap_list[i], mod))
            return mod_ch_name[i];
    }
    return _not_found;
}

void
alarm_email_send(const char *subject, const char *content)
{
    char cmd[512] = {0};
    snprintf(cmd, sizeof(cmd) - 1,
             "sys-manager system --alarm --email --send"
             "--subject '%s' --content '%s' >/dev/null",
             subject, content);

    system(cmd);
}

#define CPU_TEMP_REBOOT 95
#define CPU_TEMP_POWEROFF 100

int
_value_check_error(sys_capture_t *cap, char *msg)
{
    int value = cap->_capture(msg);

#ifdef _DEBUG
    printf(" value: %d, min: %d, max: %d\n", value, cap->min_thr, cap->max_thr);
#endif

    if (strcmp(cap->name, "cpu-temp") == 0) {
        if (value > CPU_TEMP_REBOOT) {
            sprintf(msg, "当前值 %d", value);
            if (value > CPU_TEMP_POWEROFF)
                return VAL_EMERG;
            else
                return VAL_CRIT;
        }
    }

    if (value < cap->min_thr) {
        sprintf(msg, "当前值 %d, 未达到最低值 %d", value, cap->min_thr);
        return VAL_ERROR;
    } else if (value > cap->max_thr) {
        sprintf(msg, "当前值 %d, 已超过最高值 %d", value, cap->max_thr);
        return VAL_ERROR;
    }

    return VAL_NORMAL;
}

void
_capture(sys_capture_t *cap)
{
    char msg[256]     = {0};
    char log_msg[256] = {0};
    int  _cur_error   = VAL_ERROR;

#ifndef _DEBUG
    if (!isExpried(cap))
        return;
    update(cap)
#else
    printf(" handler: %p\n", cap->_capture);
#endif

        if (!cap->_capture)
    {
        syslog(LOG_NOTICE,
               "the capture %s function is invalid,"
               "no more value can be captured.",
               cap->name);
        return;
    }

    if (cap->_preset) {
        _cur_error = _value_check_error(cap, msg);
    } else {
        _cur_error = cap->_capture(msg);
    }

    switch (_cur_error) {
    case VAL_EMERG:
        sprintf(log_msg, "监控模块%s告警: %s, 自动关闭系统", MOD_NAME(cap->name), msg);
        LogInsert(NULL, "SysMon", "Auto", "Error", log_msg);
        sprintf(msg, "监控模块%s告警", MOD_NAME(cap->name));
        alarm_email_send(msg, log_msg);
        system("poweroff&");
        break;
    case VAL_CRIT:
        sprintf(log_msg, "监控模块%s告警: %s, 自动重启系统", MOD_NAME(cap->name), msg);
        LogInsert(NULL, "SysMon", "Auto", "Error", log_msg);
        sprintf(msg, "监控模块%s告警", MOD_NAME(cap->name));
        alarm_email_send(msg, log_msg);
        system("reboot&");
        break;
    case VAL_WARNING:
        if (VAL_NORMAL == cap->_error) {
            cap->_error = VAL_WARNING;
            sprintf(log_msg, "监控模块%s告警: %s", MOD_NAME(cap->name), msg);
            LogInsert(NULL, "SysMon", "Auto", "Warning", log_msg);
        }
        break;
    case VAL_ERROR:
        if (cap->_error != VAL_ERROR) {
            cap->_error = VAL_ERROR;
            sysmon_event("self_run", "env_exception_raise", cap->name, msg);
            sprintf(log_msg, "监控模块%s告警: %s", MOD_NAME(cap->name), msg);
            LogInsert(NULL, "SysMon", "Auto", "Error", log_msg);
            sprintf(msg, "监控模块%s告警", MOD_NAME(cap->name));
            alarm_email_send(msg, log_msg);
        }
        break;
    case VAL_NORMAL:
        if (cap->_error != VAL_NORMAL) {
            if (VAL_ERROR == cap->_error)
                sysmon_event("self_run", "env_exception_backout", cap->name, "good");
            cap->_error = VAL_NORMAL;
            sprintf(log_msg, "监控模块%s告警解除", MOD_NAME(cap->name));
            LogInsert(NULL, "SysMon", "Auto", "Error", log_msg);
            alarm_email_send(log_msg, log_msg);
        }
    default:
        break;
    }
}

void
_check_interval()
{
    struct list *  n, *nt;
    sys_capture_t *cap;

#ifdef _DEBUG
    printf("enter _check_interval()\n");
#endif
    if (unlikely(global_print_on)) {
        static int print_header = 1;
        time_t     now_t        = time(NULL);
        struct tm  now_tm;
        localtime_r(&now_t, &now_tm);
        if (print_header) {
            printf("时间， cpu温度， 机箱温度， 机箱风扇1， 机箱风扇2，"
                   "电源1状态， 电源1输入电压， 电源1输出电压， 电源1风扇转速,"
                   "电源1环温， 电源1热点温度,"
                   "电源2状态， 电源2输入电压， 电源2输出电压， 电源2坟山转速,"
                   "电源2环温， 电源2热点温度\n");
            print_header = 0;
        }

        printf("%d%02d%02d-%02d%02d%02d,", now_tm.tm_year + 1900, now_tm.tm_mon + 1, now_tm.tm_mday,
               now_tm.tm_hour, now_tm.tm_min, now_tm.tm_sec);
    }

    list_iterate_safe(n, nt, &_g_capture)
    {
        cap = list_struct_base(n, sys_capture_t, list);
#ifdef _DEBUG
        printf("capture: %s", cap->name);
#endif
        _capture(cap);
    }

    if (unlikely(global_print_on)) {
        printf("\n");
    }
}

void
do_interval_check(int sig)
{
    if (sig == SIGALRM) {
        signal(SIGALRM, SIG_IGN);
        _check_interval();
        signal(SIGALRM, do_interval_check);
        alarm(CHECK_INTVAL);
    }
}

void
dump_self_run()
{
    struct list *  n, *nt;
    sys_capture_t *cap;

    puts("-----------------dump capture-----------------\n");
    list_iterate_safe(n, nt, &_g_capture)
    {
        cap = list_struct_base(n, sys_capture_t, list);
        printf("caputer: %p %s\n", cap, cap->name);
        printf("\tcheck interval: %d\n", cap->check_intval);
        printf("\tmin: %d\n", cap->min_thr);
        printf("\tmax: %d\n", cap->max_thr);
        printf("\t_error: %d\n", cap->_error);
        printf("\t_preset: %d\n", cap->_preset);
    }
}
