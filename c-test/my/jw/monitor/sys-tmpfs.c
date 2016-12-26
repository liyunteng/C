/*******************************************************************************
 * Author : liyunteng
 * Email : li_yunteng@163.com
 * Created Time : 2014-02-19 16:37
 * Filename : sys-tmpfs.c
 * Description : 
 * *****************************************************************************/
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>
#include <sys/param.h>
#include <time.h>
#include <limits.h>
#include <sys/stat.h>
#include "sys-global.h"
#include "../common/jw-unistd.h"
#include "debug.h"

#define TMPFS_MSG_ROOT "./message"
#define _SORTED_ROOT TMPFS_MSG_ROOT"/sorted_all/"
#define TMPFS_ALARM_ROOT TMPFS_MSG_ROOT"/alarm/"

#define ACCESSPERMS 0777
int mkdir_p(const char *path)
{
	int n;
	char buf[PATH_MAX], *p = buf;

	assert(path);

	if ((n = strlen(path)) >= PATH_MAX) {
		DBGP("over length: %d, %d", n, PATH_MAX);
		return -1;
	}

	strcpy(buf, path);
	while ((p = strchr(p+1, '/'))) {
		struct stat sb;
		*p = '\0';
		if (stat(buf, &sb) && mkdir(buf, ACCESSPERMS)) {
			DBGE("stat/create '%s' failed", buf);
			return -1;
		}
		*p = '/';
	}

	return n;
}

#define DIRPTR _ptr
#define STAT _stat
#define _DIR_LIST_BEGIN(dir)	\
	do {			\
		DIR *_dir;	\
		struct dirent *_ptr;	\
		struct stat _stat;	\
		char _file[PATH_MAX];	\
		if ( (_dir=opendir(dir)))	\
		{		\
			while ( (_ptr=readdir(_dir)))	\
			{	\
				sprintf(_file, "%s%s", dir, DIRPTR->d_name);	\
				if (lstat(_file, &_stat))	\
				continue;
#define _DIR_LIST_END	\
			}	\
			closedir(_dir);	\
		}	\
	} while(0);



int tmpfs_msg_count(const char *level)
{
	char msg_dir[PATH_MAX];
	int _count=0;

	if (level) {
		sprintf(msg_dir, "%s/%s/", TMPFS_MSG_ROOT, level);

		_DIR_LIST_BEGIN(msg_dir) {
			if (!S_ISREG(STAT.st_mode) && !S_ISLNK(STAT.st_mode))
				continue;
			_count++;
		}

		_DIR_LIST_END
	}
	return _count;
}

int _dir_max_file_num(const char *dir_path)
{
	int max = -1;

	_DIR_LIST_BEGIN(dir_path) {
		if (!S_ISREG(STAT.st_mode) && !S_ISLNK(STAT.st_mode))
			continue;
		if (atoi(DIRPTR->d_name) > max)
			max = atoi(DIRPTR->d_name);
	}

	_DIR_LIST_END
		return (max+1);
}


const char *tmpfs_msg_insert(const char *level, const char *msg)
{
	char msg_dir[PATH_MAX], msg_content[1024];
	static char msg_file[PATH_MAX];
	int fd;

	sprintf(msg_dir, "%s/%s/", TMPFS_MSG_ROOT, level);
	if (!mkdir_p(msg_dir)) {
		return NULL;
	}

	sprintf(msg_file, "%s%d", msg_dir, _dir_max_file_num(msg_dir));
	if ( (fd=open(msg_file, O_CREAT|O_RDWR|O_TRUNC, S_IRWXU)) > 0) {
		time_t t = time(NULL);
		sprintf(msg_content, "%s%s", ctime(&t), msg);
		*(strchr(msg_content, '\n')) = '|';
		write(fd, msg_content, strlen(msg_content));
		close(fd);
		return msg_file;
	}

	return NULL;
}


const char *tmpfs_msg_remove_oldest(const char *level)
{
	time_t oldest = 0; 
	bool o_set = false;
	char msg_dir[PATH_MAX];
	static char oldest_file[PATH_MAX] = {0};

	sprintf(msg_dir, "%s/%s/", TMPFS_MSG_ROOT, level);

	_DIR_LIST_BEGIN(msg_dir) {
		if (!S_ISREG(STAT.st_mode) && !S_ISLNK(STAT.st_mode))
			continue;

		if (!o_set) {
			oldest = STAT.st_mtime;
			o_set = true;
		}

		if (STAT.st_mtime <= oldest) {
			sprintf(oldest_file, "%s%s", msg_dir, DIRPTR->d_name);
			oldest = STAT.st_mtime;
		}


	}
	_DIR_LIST_END

		if ( (oldest_file[0] != '\0') && !unlink(oldest_file)) {
			return oldest_file;
		}

	return NULL;

}


ssize_t tmpfs_msg_sorted_link(const char *file)
{
	char link[PATH_MAX];

	if (file && mkdir_p(_SORTED_ROOT)){
		sprintf(link, "%s%d", _SORTED_ROOT, _dir_max_file_num(_SORTED_ROOT));
		return symlink(file, link);
	}

	return -1;
}

void tmpfs_write_alarm(const char *fname, const char *msg)
{
	char fpath[PATH_MAX];
	int fd;

	if (fname && mkdir_p(TMPFS_ALARM_ROOT)) {
		sprintf(fpath, "%s%s", TMPFS_ALARM_ROOT, fname);
#ifdef _DEBUG
		printf("fpath: %s\n", fpath);
#endif

		if ( (fd=open(fpath, O_CREAT|O_TRUNC|O_RDWR, 
						S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)) > 0) {

			write(fd, msg, strlen(msg));
			close(fd);
		}
	}
}


ssize_t tmpfs_msg_sorted_unlink(const char *file)
{
	ssize_t ret = -1;

	if (!file)
		return ret;

	_DIR_LIST_BEGIN(_SORTED_ROOT) {
		if (!S_ISLNK(STAT.st_mode))
			continue;

		char _link[PATH_MAX], _file[PATH_MAX];
		ssize_t _sz;

		sprintf(_link, "%s%s", _SORTED_ROOT, DIRPTR->d_name);
		if ( (_sz = readlink(_link, _file, sizeof(_file))) > 0) {
			_file[_sz] = '\0';
			if (!strcmp(file, _file)) {
				ret = unlink(_link);
				break;
			}
		}
	}
	_DIR_LIST_END
		return ret;
}
