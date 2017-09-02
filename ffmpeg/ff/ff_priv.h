/*
 * Filename: ff_priv.h
 * Description: priv
 *
 * Copyright (C) 2017 StreamOcean
 *
 * Author: liyunteng <liyunteng@streamocean.com>
 * License: StreamOcean
 * Last-Updated: 2017/09/02 01:04:25
 */
#ifndef FF_PRIV_H
#define FF_PRIV_H

#define URL_MAX_LEN 256
#define CMD_MAX_LEN 1024

#define LOGPREFIX "[%s:%d] "
#define LOGPOSTFIX "\n"
#define LOGVAR __FUNCTION__, __LINE__
#define DEBUG(format, ...)   printf(LOGPREFIX format LOGPOSTFIX, LOGVAR, ##__VA_ARGS__)
#define INFO(format, ...)    printf(LOGPREFIX format LOGPOSTFIX, LOGVAR, ##__VA_ARGS__)
#define WARNING(format, ...) printf(LOGPREFIX format LOGPOSTFIX, LOGVAR, ##__VA_ARGS__)
#define ERROR(format, ...)   printf(LOGPREFIX format LOGPOSTFIX, LOGVAR, ##__VA_ARGS__)

#endif
