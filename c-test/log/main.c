/*
 * main.c -- test log
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/12/16 22:13:38
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include "log.h"
volatile int quit = 1;
int64_t count = 0;

void handler (int signo)
{
        if (signo == SIGALRM) {
                quit = 0;
        }
}
void *run(void *arg)
{
        char buf[256];
        snprintf(buf, 256, "%lu.log", (unsigned long)pthread_self());
        loger *loger = log_create("default.log", LOG_DEBUG);
        struct itimerval itv;

        itv.it_interval.tv_sec = 0;
        itv.it_interval.tv_usec = 0;
        itv.it_value.tv_sec = 1;
        itv.it_value.tv_usec = 0;

        signal(SIGALRM, handler);
        if (setitimer(ITIMER_REAL, &itv, NULL) != 0) {
                fprintf(stderr, "setitimer failed.");
                return (void *)-1;
        }
#if 1
        while (quit) {
                DEBUG(loger, "%lu: this is debug", (unsigned long)pthread_self());
                INFO(loger, "%lu: this is info", (unsigned long)pthread_self());
                WARNING(loger, "%lu: this is a warn", (unsigned long)pthread_self());
                ERROR(loger, "%lu: this is error", (unsigned long)pthread_self());
                FATAL(loger, "%lu: this is fatal", (unsigned long)pthread_self());

                /*
                 * LOG(LOG_DEBUG, "%lu: this is debug", (unsigned long)pthread_self());
                 * LOG(LOG_INFO, "%lu: this is info", (unsigned long)pthread_self());
                 * LOG(LOG_WARNING, "%lu: this is warning", (unsigned long)pthread_self());
                 * LOG(LOG_ERROR, "%lu: this is error", (unsigned long)pthread_self());
                 * LOG(LOG_FATAL, "%lu: this is fatal", (unsigned long)pthread_self());
                 */

        }
        printf("%lu: done\n", (unsigned long)pthread_self());
#endif
        return (void*)0;

}
#if 0
int main(int argc, char *argv[])
{
        int i;
        loger *loger = log_create("test.log", LOGLEVEL_ERROR);
        /*
         * char buf[4098];
         * memset(buf, 'a', sizeof(buf));
         * buf[4096] = 'b';
         * buf[4097] = '\0';
         */
        /* LOG_INIT("test.log", LOG_ERROR); */
        struct itimerval itv;

        itv.it_interval.tv_sec = 0;
        itv.it_interval.tv_usec = 0;
        itv.it_value.tv_sec = 1;
        itv.it_value.tv_usec = 0;

        signal(SIGALRM, handler);
        if (setitimer(ITIMER_REAL, &itv, NULL) != 0) {
                fprintf(stderr, "setitimer failed.");
                return -1;
        }

        int count = 0;
        while (quit) {
                        DEBUG(loger, "%lu: this is debug", (unsigned long)pthread_self());
                        INFO(loger, "%lu: this is info", (unsigned long)pthread_self());
                        WARNING(loger, "%lu: this is a warn", (unsigned long)pthread_self());
                        ERROR(loger, "%lu: this is error", (unsigned long)pthread_self());
                        FATAL(loger, "%lu: this is fatal", (unsigned long)pthread_self());
                        ALERT(loger, "%lu: this is alert", (unsigned long)pthread_self());
                        EMERG(loger, "%Lu: this is emerg", (unsigned long)pthread_self());

                        /*
                         * LOG(LOG_DEBUG, "%s", buf);
                         * LOG(LOG_INFO, "%s", buf);
                         * LOG(LOG_ERROR, "%s", buf);
                         */
                        /*
                         * LOG(LOG_DEBUG, "%lu: this is debug", (unsigned long)pthread_self());
                         * LOG(LOG_INFO, "%lu: this is info", (unsigned long)pthread_self());
                         * LOG(LOG_WARNING, "%lu: this is warning", (unsigned long)pthread_self());
                         * LOG(LOG_ERROR, "%lu: this is error", (unsigned long)pthread_self());
                         * LOG(LOG_FATAL, "%lu: this is fatal", (unsigned long)pthread_self());
                         * LOG(LOG_ALERT, "%lu: this is alert", (unsigned long)pthread_self());
                         * LOG(LOG_EMERG, "%lu: this is emerg", (unsigned long)pthread_self());
                         */

                        count++;
        }

        printf("count: %d\n", count);
        log_dump();
        return 0;
}
#endif
#if 1
int main(int argc, char *argv[])
{
        pthread_t pids[4];
        int i;
        LOG_INIT("test.log", LOGLEVEL_INFO);

        for (i = 0; i < 4; i++) {
                if (pthread_create(&pids[i], NULL,
                                   run, NULL) != 0) {
                        perror("pthread_create");
                        return -1;
                }
        }

        struct itimerval itv;

        itv.it_interval.tv_sec = 0;
        itv.it_interval.tv_usec = 0;
        itv.it_value.tv_sec = 1;
        itv.it_value.tv_usec = 0;

        signal(SIGALRM, handler);
        if (setitimer(ITIMER_REAL, &itv, NULL) != 0) {
                fprintf(stderr, "setitimer failed.");
                return -1;
        }
#if 1
        while (quit) {
                /*
                 * DEBUG(loger, "%lu: this is debug", (unsigned long)pthread_self());
                 * INFO(loger, "%lu: this is info", (unsigned long)pthread_self());
                 * WARNING(loger, "%lu: this is a warn", (unsigned long)pthread_self());
                 * ERROR(loger, "%lu: this is error", (unsigned long)pthread_self());
                 * FATAL(loger, "%lu: this is fatal", (unsigned long)pthread_self());
                 */

                LOG(LOG_DEBUG, "%lu: this is debug", (unsigned long)pthread_self());
                LOG(LOG_INFO, "%lu: this is info", (unsigned long)pthread_self());
                LOG(LOG_WARNING, "%lu: this is warning", (unsigned long)pthread_self());
                LOG(LOG_ERROR, "%lu: this is error", (unsigned long)pthread_self());
                LOG(LOG_FATAL, "%lu: this is fatal", (unsigned long)pthread_self());

        }
        printf("%lu: done\n", (unsigned long)pthread_self());
#endif

#if 0
        while (quit) {
                time_t t = time(NULL);
                struct tm tm;
                localtime_r(&t,&tm);
                // asctime(gmtime(&t));
                count++;
        }
        fprintf(stderr, "count: %lu", count);
#endif
        for (i = 0; i < 4; i++)
                pthread_join(pids[i], NULL);

        log_dump();
        return 0;

}
#endif
