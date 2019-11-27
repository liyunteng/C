/*******************************************************************************
 * Author : liyunteng
 * Email : li_yunteng@163.com
 * Created Time : 2014-02-19 15:08
 * Filename : sys-capture.c
 * Description :
 * *****************************************************************************/
#include "pmu-info.h"
#include "sys-interval-check.h"
#include "sys-mon.h"
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define NCT_ROOT "/sys/devices/platform/nct6106.656"

const char *mod_cap_list[] = {"cpu-temp",  "env-temp", "case-temp", "case-fan1",
                              "case-fan2", "cpu-fan",  "power",     NULL};

const char *mod_ch_name[] = {"CPU温度",   "环境温度", "机箱温度", "机箱风扇1",
                             "机箱风扇2", "CPU风扇",  "电源",     NULL};

bool
isCaptureSupported(const char *mod)
{
    int i;

    for (i = 0; mod_cap_list[i]; i++) {
        if (!strcmp(mod_cap_list[i], mod))
            return true;
    }

    return false;
}

const char *
__read_file_line(const char *file)
{
    static char line[1024];
    char *p;
    int fd;

    line[0] = '\0';
    if ((fd = open(file, O_RDONLY)) > 0) {
        read(fd, line, sizeof(line) - 1);
        p = strstr(line, "\n");
        if (p)
            *p = '\0';
        close(fd);
    } else {
        syslog(LOG_NOTICE, "%s: fail to read %s", __func__, file);
        return NULL;
    }

    return line;
}

int
__atoi(const char *p)
{
    int tmp = -1;

    if (p)
        tmp = atoi(p);
    return tmp;
}

int
__read_int_value(const char *file)
{
    return __atoi(__read_file_line(file));
}

int
capture_cpu_temp(char *msg)
{
    int val = __read_int_value(NCT_ROOT "/temp17_input");
    if (unlikely(global_print_on)) {
        printf("%d, ", val > 0 ? val / 1000 : -1);
    }
    if (val > 0)
        return (int)(val / 1000);
    return val;
}

int
capture_env_temp(char *msg)
{
    int val = __read_int_value(NCT_ROOT "/temp20_input");
    if (val > 0)
        return (int)(val / 1000);

    return val;
}

int global_case_temp = -1;

int
capture_case_temp(char *msg)
{
    int val = __read_int_value(NCT_ROOT "/temp18_input");
    if (unlikely(global_print_on)) {
        printf("%d, ", val > 0 ? val / 1000 : -1);
    }

    if (val > 0) {
        global_case_temp = (int)(val / 1000);
        return (int)(val / 1000);
    } else {
        global_case_temp = -1;
    }

    return val;
}

int
capture_case_fan1(char *msg)
{
    int val = __read_int_value(NCT_ROOT "/fan1_input");
    if (unlikely(global_print_on)) {
        printf("%d, ", val);
    }

    return val;
}

int
capture_case_fan2(char *msg)
{
    int val = __read_int_value(NCT_ROOT "/fan3_input");
    if (unlikely(global_print_on)) {
        printf("%d, ", val);
    }
    return val;
}

int
capture_cpu_fan(char *msg)
{
    int val = __read_int_value(NCT_ROOT "/fan2_input");
    if (unlikely(global_print_on)) {
        printf("%d, ", val);
    }

    return val;
}

int
_power_check(int module_no, struct pmu_info *info, char *msg)
{
    if (info->is_vin_fault || info->is_vout_fault || info->is_temp_fault
        || info->is_fan_fault) {
        sprintf(msg, "%s电源模块%d%s%s%s%s异常", msg, module_no,
                info->is_vin_fault ? "输入电压" : "",
                info->is_vout_fault ? "输出电压" : "",
                info->is_fan_fault ? "风扇" : "",
                info->is_temp_fault ? "温度" : "");
        return -1;
    } else
        return 0;
}

#define POWER_TEMP_CHECK_INTERVAL 180

int
capture_power(char *msg)
{
    int fail_cnt = 0;
    struct pmu_info info;
    int check_temp          = 0;
    static time_t last_time = 0;

    if (!msg)
        return VAL_ERROR;

    msg[0] = '\0';
    memset(&info, 0x0, sizeof(info));
    if (time(NULL) - last_time > POWER_TEMP_CHECK_INTERVAL)
        check_temp = 1;
    if (!pmu_get_info(PMU_DEV1, &info, check_temp)) {
        if (_power_check(1, &info, msg) != 0)
            fail_cnt++;
    } else
        fail_cnt++;

    memset(&info, 0x0, sizeof(info));
    if (!pmu_get_info(PMU_DEV2, &info, check_temp)) {
        if (_power_check(2, &info, msg) != 0)
            fail_cnt++;
    } else
        fail_cnt++;

    if (check_temp)
        last_time = time(NULL);

    if (fail_cnt > 0) {
        if (2 == gconf.power_cnt) {
            return VAL_ERROR;
        } else
            return VAL_WARNING;
    }

    return VAL_NORMAL;
}

capture_func
capture_get(const char *mod)
{
    if (!isCaptureSupported(mod))
        return NULL;

    if (!strcmp(mod, "cpu-temp"))
        return capture_cpu_temp;

    else if (!strcmp(mod, "env-temp"))
        return capture_env_temp;
    else if (!strcmp(mod, "case-temp"))
        return capture_case_temp;
    else if (!strcmp(mod, "case-fan1"))
        return capture_case_fan1;
    else if (!strcmp(mod, "case-fan2"))
        return capture_case_fan2;
    else if (!strcmp(mod, "cpu-fan"))
        return capture_cpu_fan;
    else if (!strcmp(mod, "power"))
        return capture_power;
    return NULL;
}

void
sys_capture_init()
{
    list_init(&_g_capture);
}

void
sys_capture_release()
{
    struct list *n, *nt;
    sys_capture_t *c;

    list_iterate_safe(n, nt, &_g_capture)
    {
        c = list_struct_base(n, sys_capture_t, list);
        list_del(&c->list);
        free(c);
    }
}

sys_capture_t *
sys_capture_alloc()
{
    sys_capture_t *tmp;

    tmp = (sys_capture_t *)malloc(sizeof(sys_capture_t));

    if (tmp) {
        tmp->name[0]      = '\0';
        tmp->check_intval = 0;
        tmp->min_thr = tmp->max_thr = 0;
        tmp->_last_update           = time(NULL);
        tmp->_capture               = NULL;
        tmp->_error                 = VAL_NORMAL;
        tmp->_preset                = false;
    }

    return tmp;
}

void
sys_capture_set_handler(sys_capture_t *cap)
{
    if (isCaptureSupported(cap->name))
        cap->_capture = capture_get(cap->name);
}

void
sys_capture_add(sys_capture_t *cap)
{
    if (cap)
        list_add(&_g_capture, &cap->list);
}
