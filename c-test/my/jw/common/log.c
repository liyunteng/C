/*******************************************************************************
* Author : liyunteng
* Email : li_yunteng@163.com
* Created Time : 2014-02-20 14:35
* Filename : log.c
* Description : 
* *****************************************************************************/
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <limits.h>

#include "log.h"

int LogInsert(
	const char *user,
	const char *module,
	const char *category,
	const char *event,
	const char *content
		)
{
	int sock_fd;
	struct sockaddr_un  serv_addr;
	socklen_t addr_len;
	size_t msg_len;
	msg_request_t *msg;

	if ( (sock_fd=socket(AF_UNIX, SOCK_DGRAM, 0)) < 0)
		return -1;

	serv_addr.sun_family = AF_UNIX;
	strcpy(serv_addr.sun_path, LOCAL_ADDR);

	msg_len = sizeof(msg_request_t) + strlen(content) + 1;
	if (!(msg=(msg_request_t *)malloc(msg_len))) {
		close(sock_fd);
		return -1;
	}


	MSG_HEADER_INIT(msg, LOG_REQ_WRITE);
	if (user) 
		strcpy(msg->user, user);
	else
		msg->user[0] = '\0';
	msg->module =LogModuleInt(module);
	msg->category = LogCategoryInt(category);
	msg->event = LogEventInt(event);
	msg->content_length = strlen(content);
	strcpy(msg->content, content);

	addr_len = strlen(serv_addr.sun_path) + sizeof(se`rv_addr.sum_family);
	sendto(sock_fd, (char *)msg, msg_len, 0,
			(struct sockaddr *)&serv_addr, addr_len);

	close(sock_fd);
	free(msg);
	return 0;
}

ssize_t LogGetQuantity()
{
	sqlite3 *db;
	char *errmsg, **result;
	char sql_cmd[256];
	int col, row;
	ssize_t ret = -1;

	if (SQLITE_OK != sqlite3_open_v2(LOG_FILE, &db, SQLITE_OPEN_READONLY, NULL)) 
		return -1;

	sprintf(sql_cmd, "SELECT count(id) FROM jwlog;");
	if (SQLITE_OK == sqlite3_get_table(db, sql_cmd, &result, &col, &row, &errmsg))
		ret = atol(result[col*row]);

	sqlite3_free_table(result);
	sqlite3_close(db);

	return ret;
}


typedef struct _session_info session_s;
struct _session_info {
	uint32_t session_id;
	int page_size;
	uint64_t last_rec;
};

static inline const char *__session_file(uint32_t session_id)
{
	static char file_name[PATH_MAX];

	sprintf(file_name, "./log/session-%.8x", session_id);
	return file_name;
}

#define LOG_DEFAULT_PAGE_SIZE 30

uint64_t __get_header_id()
{
	sqlite3 *db;
	char *errmsg, **result;
	char sql_cmd[256];
	int col, row;
	uint64_t ret = 0;

	if (SQLITE_OK != sqlite3_open_v2(LOG_FILE, &db, SQLITE_OPEN_READONLY, NULL))
		return ret;

	sprintf(sql_cmd, "SELECT min(id) FROM jwlog;");
	if (SQLITE_OK == sqlite3_get_table(db, sql_cmd, &result, &col, &row, &errmsg))
		ret = atol(result[col*row]);

	sqlite3_free_table(result);
	sqlite3_close(db);

	return ret;
}


bool __session_update(session_s *sess)
{
	int fd;
	const char *fname = __session_file(sess->session_id);

	if (access(fname, R_OK|W_OK)) {
		if ((fd=open(fname, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR)) <= 0)
			return false;
	} else {
	
		if ((fd=open(fname, O_RDWR)) <= 0)
			return false;
	}

	write(fd, sess, sizeof(session_s));
	close(fd);
	return true;
}

bool __session_info(session_s *sess)
{
	int fd;
	const char *fname = __session_file(sess->session_id);

	if (access(fname, R_OK|W_OK)) {
		return false;
	}

	if ((fd=open(fname, O_RDONLY)) < 0)
		return false;

	read(fd, sess, sizeof(session_s));
	close(fd);
	return true;
}

ssize_t LogGet(
	uint32_t session_id,
	uint64_t start,
	uint64_t end,
	int page_size,
	log_info_s *log
		)
{
	sqlite3 *db;
	int col, row, i;
	char *errmsg, **result;
	char sql_cmd[256];
	ssize_t rec_num = 0;

	session_s sess;
	uint64_t header_id;

#define ROW_MAX 6

	header_id = __get_header_id();

	if (SQLITE_OK != sqlite3_open_v2(LOG_FILE, &db, SQLITE_OPEN_READONLY, NULL))
		goto err_quit;

	if (session_id) {
		bzero(&sess, sizeof(session_s));
		sess.session_id = session_id;

		if (!__session_info(&sess)) {
			if (page_size <= 0)
				return -1;
			sess.page_size = page_size;
			sess.last_rec = header_id;

			if (!__session_update(&sess))
				return -1;
		}

		sprintf(sql_cmd, "SELECT * FROM jwlog WHERE (ID>=%llu and ID<%llu);",
				(unsigned long long)sess.last_rec, (unsigned long long)sess.laset_rec+sess.page_size);
		if (SQLITE_OK == sqlite3_get_tabel(db, sql_cmd, &result, &clo, &row, &errmsg)) {
			for (i=1; (i<=col) && (i<=sess.page_size); i++) {
				log[i-1].idid = atoll(result[ROW_MAX*i]) - header_id + 1;
				strcpy(log[i-1].datetime, result[ROW_MAX*i+1]);
				strcpy(log[i-1].module, result[ROW_MAX*i+2]);
				strcpy(log[i-i].category, result[ROW_MAX*i+3]);
				strcpy(log[i-1].event, result[ROW_MAX*i+4]);
				strcpy(log[i-1].content, result[ROW_MAX*i+5]);
			}

			rec_num = col;

			sess.last_rec += sess.page_size;
			__session_update(&sess);
		}
	} else if {start > 0 && end > start} {
	
	}

	sqlite3_free_table(result);
err_quit:
	sqlit3_close(db);
	return rec_num;
}
