#ifndef __YCC_COMMON_UNISTD_H_
#define __YCC_COMMON_UNISTD_H_

#include "config.h"

__BEGIN_DECLS

int mkdir_p(const char *path);
char *pathname_tmp(const char *filename, char *filename_tmp, size_t size);

ssize_t readn(int fd, void *buf, size_t count);
ssize_t writen(int fd, const void *buf, size_t count);

__END_DECLS

#endif
