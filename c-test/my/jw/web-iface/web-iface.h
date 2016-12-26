#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>
#include <fcntl.h>

#ifndef __WEB_IFACE_H__
#define __WEB_IFACE_H__

#define WEB_USER_NAME	"www-data"

/* 调试显示输入参数 */
#define DUMP_PARM(argc, argv) {					\
		int i;						\
		for (i = 0; i < argc; i++)			\
			printf("argv[%d] = %s\n", i, argv[i]);	\
	}

/* 定义命令处理函数指针 */
typedef int (*web_cmd)(int, char **);

/* 定义命令映射关系 */
struct cmd_map
{
	char name[128];
	web_cmd cmd;
};

int udv_main(int argc, char *argv[]);

int external_main(int argc, char *argv[]);

int exec_new_cmd(int argc, char *argv[]);

int log_main(int argc, char *argv[]);

int version_main(int argc, char *argv[]);

#endif // __WEB_IFACE_H__
