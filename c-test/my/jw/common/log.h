#ifndef _SYS_LOG_H
#define _SYS_LOG_H

#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include "debug.h"

#define LOG_INVALID_ARG -1
#define LOG_FILE "/opt/log/jw-log.db"
#define LOG_TABLE "jwlog"
#define LOCAL_ADDR "/tmp/.log_socket_do_not_remove"


extern const char *_mod_name[];
extern const char *_mod_category[];
extern const char *_mod_event[];

static inline const char *_IntToStr(const char *name[], int value)
{
	int x = 0;
	while(name[x] != NULL)
	{
		if (x==value)
			return name[x];
		x++;
	}
	return "N/A";
}

static inline const int _StrToInt(const char *name[], const char *value)
{
	int x = 0;
	while(name[x] != NULL)
	{
		if (!strcmp(name[x], value))
			return x;
		x++;
	}
	return -1;
}


/*--------------------------------------------------------------------------- */

typedef enum _module log_module_e;
enum _module
{
	LOG_MOD_WEB = 0,
	LOG_MOD_DISK,
	LOG_MOD_VG,
	LOG_MOD_UDV,
	LOG_MOD_ISCSI,
	LOG_MOD_NAS,
	LOG_MOD_SYSCONF,
	LOG_MOD_SYSMON,
	LOG_MOD_UNKNOWN = -1
};

static inline const char *LogModuleStr(log_module_e module)
{
	DBGP("%s : %d, %s\n", __func__, module, _IntToStr(_mod_name, module));
	return _IntToStr(_mod_name, module);
}

static inline const int LogModuleInt(const char *module)
{
	DBGP("%s : %s %d\n", __func__, module, _StrToInt(_mod_name, module));
	return _StrToInt(_mod_name, module);
}

/*--------------------------------------------------------------------------- */

typedef enum _category log_category_e;
enum _category
{
	LOG_CATG_AUTO = 0,
	LOG_CATG_MANUAL,
	LOG_CATG_UNKNOWN = -1
};

static inline const char *LogCategoryStr(log_category_e category)
{
	return _IntToStr(_mod_category, category);
}

static inline const int LogCategoryInt(const char *category)
{
	return _StrToInt(_mod_category, category);
}

/*--------------------------------------------------------------------------- */

typedef enum _event log_event_e;
enum _event
{
	LOG_EV_INFO = 0,
	LOG_EV_WARNING,
	LOG_EV_ERROR,
	LOG_EV_UNKNOWN = -1
};

static inline const char *LogEventStr(log_event_e event)
{
	return _IntToStr(_mod_event, event);
}

static inline const int LogEventInt(const char *event)
{
	return _StrToInt(_mod_event, event);
}

/*--------------------------------------------------------------------------- */

typedef struct _log_stru log_info_s;
struct _log_stru
{
	uint64_t idid;
	char datetime[32];
	char module[32];
	char category[32];
	char event[32];
	char content[1024];
};


#define LOG_MAGIC 0x915A925A
struct _msg_header {
	uint32_t magic;
	int req_mode;
};

enum {
	LOG_REQ_WRITE = 1,
	LOG_REQ_UNKNOW
};

#define MSG_HEADER_INIT(msg, req)	\
	msg->magic = LOG_MAGIC;		\
	msg->req_mode = req;	

#define MSG_HEADER_CORRECT(msg)		\
	(msg->magic == LOG_MAGIC)

typedef struct _msg_request msg_request_t;
struct _msg_request {
	struct _msg_header header;

#define magic header.magic
#define req_mode header.req_mode
	log_module_e module;
	log_category_e category;
	log_event_e event;
	char user[32];
	int content_length;
	char content[0];
};



bool log_db_exist(void);
bool log_db_create(void);


int LogInsert(
	const char *user,
	const char *module,
	const char *category,
	const char *event,
	const char *content);

ssize_t LogGetQuantity(void);

ssize_t LogGet(
	uint32_t session_id,
	uint64_t start,
	uint64_t end,
	int page_size,
	log_info_s *log
		);

#endif

