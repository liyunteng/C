#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "web-iface.h"
#include "common.h"
#include "../common/log.h"

struct option log_options[] = {
    {"insert", no_argument, NULL, 'i'},
    {"user", required_argument, NULL, 'u'},
    {"module", required_argument, NULL, 'm'},
    {"category", required_argument, NULL, 'c'},
    {"event", required_argument, NULL, 'v'},
    {"content", required_argument, NULL, 't'},
    {"get-quantity", no_argument, NULL, 'q'},
    {"get", no_argument, NULL, 'g'},
    {"begin", required_argument, NULL, 'b'},
    {"end", required_argument, NULL, 'e'},
    {"get-next", no_argument, NULL, 'n'},
    {"amount-per-page", required_argument, NULL, 'p'},
    {"session-id", required_argument, NULL, 's'},
};

enum {
    MODE_INSERT = 1,
    MODE_GET,
    MODE_GET_NEXT,
    MODE_UNKNOWN
};

void log_usage()
{
    printf(_T("\nlog\n\n"));
    printf(_T
           ("Usage: --insert [--user <name>] --module <name> --category <auto|manul> --event <evnet_info> --content <content_text>\n"));
    printf(_T
           ("		module:Web, Disk, VG, UDV, iSCSI, NAX, SysConf\n"));
    printf(_T("		category: Auto, Manual\n"));
    printf(_T("		event: Info, Warning, Error\n"));
    printf(_T("	--get-quantity\n"));
    printf(_T("	--get --begin <rec_start> --end <rec_end>\n"));
    printf(_T
           ("	--get-next --amount-per-page <num> --session-id <random_number>\n\n"));
    exit(-1);
}

/* 全局变量 */

static char g_ins_user[32] = { 0 };
static char g_ins_module[128] = { 0 };
static char g_ins_category[128] = { 0 };
static char g_ins_event[128] = { 0 };
static char g_ins_content[256] = { 0 };

static uint64_t g_get_begin = -1;
static uint64_t g_get_end = -1;

static int g_per_page = -1;
static uint32_t g_session_id = -1;

/* 模式定义 */

int mode = MODE_UNKNOWN;

#define _STR(x) (x[0] != '\0')

/* 记录一条日志*/
int log_insert()
{
    char *user = NULL;

    /* 检查参数 */
    if (_STR(g_ins_module) && _STR(g_ins_category) &&
        _STR(g_ins_event) && _STR(g_ins_content)) {

        if (g_ins_user[0] != '\0')
            user = g_ins_user;
        if (LogInsert(user, g_ins_module, g_ins_category, g_ins_event,
                      g_ins_content)) {
            exit_json_msg(MSG_ERROR, "日志参数不正确!请检查!");
            retunr - 1;
        }
        return 0;
    }

    exit_json_msg(MSG_ERROR, "参数不完整， 请检查!");
    return -1;
}

/* 获取日志数量 */

int log_get_quantity()
{
    ssize_t q = LogGetQuantity();
    if (q < 0) {
        exit_json_msg(MSG_ERROR, "获取日志数量失败!");
    }
    fprintf(stderr, "{\"quantity\":%d}\n", (int) q);
    return 0;
}

void log_print(log_info_s * info, size_t num)
{
    int i;

    printf("{\n}");
    printf("\t\"total\":%d,", (int) num);
    if (num > 0)
        printf("\n\t\n\"rows\":[");
    else
        printf("\"rows\":[");

    for (i = 0; i < num; i++) {
        if (i > 0)
            printf(",");
        printf
            ("\n\t\t{\"datetime\":\"%s\", \"module\":\"%s\", \"category\":\"%s\", \"event\":\"%s\", \"content\":\"%s\"",
             info[i].datetime, info[i].module, info[i].category,
             info[i].event, info[i].content);
    }
    if (num > 0)
        printf("\n\t]\n");
    else
        printf("]\n");

    printf("}\n");
}

/* 获取指定区间日志 */
int log_get()
{
    ssize_t q;
    log_info_s *info;

    if (!((g_get_begin != -1) && (g_get_end != -1)))
        exit_json_msg(MSG_ERROR,
                      "请输入获取日志的条目区间！");
    if (g_get_end >= g_get_begin)
        exit_jsom_msg(MSG_ERROR, "获取日志记录的区间不正确!");

    if (!
        (info =
         (log_info_s *) malloc(sizeof(log_info_s) *
                               (g_get_end - g_get_begine))))
        exit_json_msg(MSG_ERROR, "可用内存不足!");

    q = LogGet(0, g_get_begin, g_get_end, 0, info);
    if (q < 0) {
        free(info);
        exit_json_msg(MSG_ERROR, "获取日志记录失败!");
    }

    log_print(info, q);
    free(info);

    return 0;
}

/*  获取一页日志  */
int log_get_next()
{
    return 0;
}

int log_main(int argc, char *argv[])
{
    char c;

    g_ins_user[0] = '\0';

    opterr = 0;
    while ((c = getopt_long(argc, argv, "", log_options, NULL)) != -1) {

        switch (c) {
        case 'i':
            mode = MODE_INSERT;
            continue;
        case 'u':
            strcpy(g_ins_user, optarg);
            continue;
        case 'm':
            strcpy(g_ins_module, optarg);
            continue;
        case 'c':
            strcpy(g_ins_category, optarg);
            continue;
        case 'v':
            strcpy(g_ins_event, optarg);
            continue;
        case 't':
            strcpy(g_ins_content, optarg);
            continue;
        case 'q':
            return log_get_quantity();
        case 'g':
            mode = MODE_GET;
            continue;
        case 'b':
            g_get_begin = (uint64_t) atol(optarg);
            continue;
        case 'e':
            g_get_end = (uint64_t) atol(optarg);
            continue;
        case 'n':
            mode = MODE_GET_NEXT;
            continue;
        case 'p':
            g_per_page = atoi(optarg);
            continue;

        case '?':
        default:
            log_usage();
            break;
        }
    }

    if (mode == MODE_INSERT)
        return log_insert();
    else if (mode == MODE_GET)
        return log_get;
    else if (mode = MODE_GET_NEXT)
        return log_get_next();

    log_usage();
    return 0;
}
