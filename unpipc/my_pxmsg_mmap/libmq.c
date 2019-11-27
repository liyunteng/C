/*
 * libmq.c--
 *
 * Copyright (C) 2014,2015,  <li_yunteng@163.com>
 * Auther: liyunteng
 * License: GPL
 * Update time:
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

#include "libmq.h"
#include "../unpipc.h"

#include <stdarg.h>
#include <sys/mman.h>

#define MAX_TRIES 10 /* for waiting for initialization */

struct mq_attr defattr = {0, 128, 1024, 0};

mqd_t
mq_open(const char *pathname, int oflag, ...)
{
    int i, fd, nonblock, created, save_errno;
    long msgsize, filesize, index;
    va_list ap;
    mode_t mode;
    int8_t *mptr;
    struct stat statbuf;
    struct mq_hdr *mqhdr;
    struct msg_hdr *msghdr;
    struct mq_attr *attr;
    struct mq_info *mqinfo;
    pthread_mutexattr_t mattr;
    pthread_condattr_t cattr;

    created  = 0;
    nonblock = oflag & O_NONBLOCK;
    oflag &= ~O_NONBLOCK;
    mptr   = (int8_t *)MAP_FAILED;
    mqinfo = NULL;

again:
    if (oflag & O_CREAT) {
        va_start(ap, oflag); /* init ap to final named
                              * arguments */
        mode = va_arg(ap, mode_t) & ~S_IXUSR;
        attr = va_arg(ap, struct mq_attr *);
        va_end(ap);

        /* open and specify O_EXCL and user-execute */
        fd = open(pathname, oflag | O_EXCL | O_RDWR, mode | S_IXUSR);
        if (fd < 0) {
            if (errno == EEXIST && (oflag & O_EXCL) == 0)
                goto exists;
            else
                return ((mqd_t)-1);
        }

        created = 1;
        /* first one to create the file initialization it */
        if (attr == NULL)
            attr = &defattr;
        else {
            if (attr->mq_maxmsg <= 0 || attr->mq_msgsize <= 0) {
                errno = EINVAL;
                goto err;
            }
        }
        /* calculate and set the file size */
        msgsize  = MSGSIZE(attr->mq_msgsize);
        filesize = sizeof(struct mq_hdr)
                   + (attr->mq_maxmsg * (sizeof(struct msg_hdr) + msgsize));

        if (lseek(fd, filesize - 1, SEEK_SET) == -1) {
            goto err;
        }
        if (write(fd, "", 1) == -1) {
            goto err;
        }

        /* memory map the file */
        mptr = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (mptr == MAP_FAILED)
            goto err;

        /* allocate on mq_info{} for the queue */
        if ((mqinfo = malloc(sizeof(struct mq_info))) == NULL)
            goto err;

        mqinfo->mqi_hdr = mqhdr = (struct mq_hdr *)mptr;
        mqinfo->mqi_magic       = MQI_MAGIC;
        mqinfo->mqi_flags       = nonblock;

        /* initialize header at beginning of file */
        /* create free list with all message on it */

        mqhdr->mqh_attr.mq_flags   = 0;
        mqhdr->mqh_attr.mq_maxmsg  = attr->mq_maxmsg;
        mqhdr->mqh_attr.mq_msgsize = attr->mq_msgsize;
        mqhdr->mqh_attr.mq_curmsgs = 0;

        mqhdr->mqh_nwait = 0;
        mqhdr->mqh_pid   = 0;
        mqhdr->mqh_head  = 0;
        index            = sizeof(struct mq_hdr);
        mqhdr->mqh_free  = index;

        for (i = 0; i < attr->mq_maxmsg - 1; i++) {
            msghdr = (struct msg_hdr *)&mptr[index];
            index += sizeof(struct msg_hdr) + msgsize;
            msghdr->msg_next = index;
        }
        msghdr           = (struct msg_hdr *)&mptr[index];
        msghdr->msg_next = 0; /* end of free list */

        /* initialize mutex & condition variable */
        if ((i = pthread_mutexattr_init(&mattr)) != 0) {
            goto pthreaderr;
        }
        pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
        i = pthread_mutex_init(&mqhdr->mqh_lock, &mattr);
        pthread_mutexattr_destroy(&mattr); /* be sure destroy
                                            * */
        if (i != 0)
            goto pthreaderr;

        if ((i = pthread_condattr_init(&cattr)) != 0) {
            goto pthreaderr;
        }
        pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED);
        i = pthread_cond_init(&mqhdr->mqh_wait, &cattr);
        pthread_condattr_destroy(&cattr);
        if (i != 0)
            goto pthreaderr;

        /* initialization complete, turn off user-execute bit */
        if (fchmod(fd, mode) == -1)
            goto err;
        close(fd);
        return ((mqd_t)mqinfo);
    }
exists:
    /* open the file then memory map */
    if ((fd = open(pathname, O_RDWR)) < 0) {
        if (errno == ENOENT && (oflag & O_CREAT))
            goto again;
        goto err;
    }

    /* make certain initialization is complete */
    for (i = 0; i < MAX_TRIES; i++) {
        if (stat(pathname, &statbuf) == -1) {
            if (errno == ENOENT && (oflag & O_CREAT)) {
                close(fd);
                goto again;
            }
            goto err;
        }

        if ((statbuf.st_mode & S_IXUSR) == 0) {
            break;
        }
        sleep(1);
    }

    if (i == MAX_TRIES) {
        errno = ETIMEDOUT;
        goto err;
    }

    filesize = statbuf.st_size;
    mptr     = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mptr == MAP_FAILED)
        goto err;
    close(fd);

    /* allocate one mq_info{} for each open */
    if ((mqinfo = malloc(sizeof(struct mq_info))) == NULL)
        goto err;
    mqinfo->mqi_hdr   = (struct mq_hdr *)mptr;
    mqinfo->mqi_magic = MQI_MAGIC;
    mqinfo->mqi_flags = nonblock;
    return ((mqd_t)mqinfo);

pthreaderr:
    errno = i;

err:
    /* don't let following function calls change errno */
    save_errno = errno;
    if (created)
        unlink(pathname);

    if (mptr != MAP_FAILED)
        munmap(mptr, filesize);

    if (mqinfo != NULL)
        free(mqinfo);

    close(fd);
    errno = save_errno;
    return ((mqd_t)-1);
}

int
mq_close(mqd_t mqd)
{
    long msgsize, filesize;
    struct mq_hdr *mqhdr;
    struct mq_attr *attr;
    struct mq_info *mqinfo;

    mqinfo = mqd;
    if (mqinfo->mqi_magic != MQI_MAGIC) {
        errno = EBADF;
        return (-1);
    }

    mqhdr = mqinfo->mqi_hdr;
    attr  = &mqhdr->mqh_attr;

    if (mq_notify(mqd, NULL) != 0) { /* unregister calling process
                                      * */
        return (-1);
    }

    msgsize  = MSGSIZE(attr->mq_msgsize);
    filesize = sizeof(struct mq_hdr)
               + (attr->mq_maxmsg * (sizeof(struct msg_hdr) + msgsize));

    if (munmap(mqinfo->mqi_hdr, filesize) == -1) {
        return (-1);
    }

    mqinfo->mqi_magic = 0;
    free(mqinfo);
    return (0);
}

int
mq_getattr(mqd_t mqd, struct mq_attr *mqstat)
{
    int n;
    struct mq_hdr *mqhdr;
    struct mq_attr *attr;
    struct mq_info *mqinfo;

    mqinfo = mqd;
    if (mqinfo->mqi_magic != MQI_MAGIC) {
        errno = EBADF;
        return (-1);
    }

    mqhdr = mqinfo->mqi_hdr;
    attr  = &mqhdr->mqh_attr;
    if ((n = pthread_mutex_lock(&mqhdr->mqh_lock)) != 0) {
        errno = n;
        return (-1);
    }

    mqstat->mq_flags   = mqinfo->mqi_flags;
    mqstat->mq_maxmsg  = attr->mq_maxmsg;
    mqstat->mq_msgsize = attr->mq_msgsize;
    mqstat->mq_curmsgs = attr->mq_curmsgs;

    pthread_mutex_unlock(&mqhdr->mqh_lock);
    return (0);
}

int
mq_notify(mqd_t mqd, const struct sigevent *notification)
{
    int n;
    pid_t pid;
    struct mq_hdr *mqhdr;
    struct mq_info *mqinfo;

    mqinfo = mqd;
    if (mqinfo->mqi_magic != MQI_MAGIC) {
        errno = EBADF;
        return (-1);
    }

    mqhdr = mqinfo->mqi_hdr;
    if ((n = pthread_mutex_lock(&mqhdr->mqh_lock)) != 0) {
        errno = n;
        return (-1);
    }

    pid = getpid();
    if (notification == NULL) {
        if (mqhdr->mqh_pid == pid) {
            mqhdr->mqh_pid = 0;
        }
    } else {
        if (mqhdr->mqh_pid != 0) {
            if (kill(mqhdr->mqh_pid, 0) != -1 || errno != ESRCH) {
                errno = EBUSY;
                goto err;
            }
        }
        mqhdr->mqh_pid   = pid;
        mqhdr->mqh_event = *notification;
    }
    pthread_mutex_unlock(&mqhdr->mqh_lock);
    return (0);
err:
    pthread_mutex_unlock(&mqhdr->mqh_lock);
    return (-1);
}

ssize_t
mq_receive(mqd_t mqd, char *ptr, size_t maxlen, unsigned int *prio)
{
    int n;
    long index;
    int8_t *mptr;
    ssize_t len;
    struct mq_hdr *mqhdr;
    struct mq_attr *attr;
    struct msg_hdr *msghdr;
    struct mq_info *mqinfo;

    mqinfo = mqd;
    if (mqinfo->mqi_magic != MQI_MAGIC) {
        errno = EBADF;
        return (-1);
    }

    mqhdr = mqinfo->mqi_hdr;
    mptr  = (int8_t *)mqhdr;
    attr  = &mqhdr->mqh_attr;
    if ((n = pthread_mutex_lock(&mqhdr->mqh_lock)) != 0) {
        errno = n;
        return (-1);
    }

    if ((long)maxlen < attr->mq_msgsize) {
        errno = EMSGSIZE;
        goto err;
    }

    if (attr->mq_curmsgs == 0) {
        if (mqinfo->mqi_flags & O_NONBLOCK) {
            errno = EAGAIN;
            goto err;
        }

        mqhdr->mqh_nwait++;
        while (attr->mq_curmsgs == 0) {
            pthread_cond_wait(&mqhdr->mqh_wait, &mqhdr->mqh_lock);
        }
        mqhdr->mqh_nwait--;
    }

    if ((index = mqhdr->mqh_head) == 0)
        err_dump("mq_receive: curmsgs = %ld; head = 0\n", attr->mq_curmsgs);

    msghdr          = (struct msg_hdr *)&mptr[index];
    mqhdr->mqh_head = msghdr->msg_next;
    len             = msghdr->msg_len;
    memcpy(ptr, msghdr + 1, len);
    if (prio != NULL) {
        *prio = msghdr->msg_prio;
    }

    msghdr->msg_next = mqhdr->mqh_free;
    mqhdr->mqh_free  = index;

    if (attr->mq_curmsgs == attr->mq_maxmsg)
        pthread_cond_signal(&mqhdr->mqh_wait);

    pthread_mutex_unlock(&mqhdr->mqh_lock);
    return (len);
err:
    pthread_mutex_unlock(&mqhdr->mqh_lock);
    return (-1);
}

int
mq_send(mqd_t mqd, const char *ptr, size_t len, unsigned int prio)
{
    int n;
    long index, freeindex;
    int8_t *mptr;
    struct sigevent *sigev;
    struct mq_hdr *mqhdr;
    struct mq_attr *attr;
    struct msg_hdr *msghdr, *nmsghdr, *pmsghdr;
    struct mq_info *mqinfo;

    mqinfo = mqd;
    if (mqinfo->mqi_magic != MQI_MAGIC) {
        errno = EBADF;
        return (-1);
    }

    mqhdr = mqinfo->mqi_hdr;
    mptr  = (int8_t *)mqhdr;
    attr  = &mqhdr->mqh_attr;

    if ((n = pthread_mutex_lock(&mqhdr->mqh_lock)) != 0) {
        errno = n;
        return (-1);
    }

    if ((long)len > attr->mq_msgsize) {
        errno = EMSGSIZE;
        goto err;
    }

    if (attr->mq_curmsgs == 0) {
        if (mqhdr->mqh_pid != 0 && mqhdr->mqh_nwait == 0) {
            sigev = &mqhdr->mqh_event;
            if (sigev->sigev_notify == SIGEV_SIGNAL) {
                sigqueue(mqhdr->mqh_pid, sigev->sigev_signo,
                         sigev->sigev_value);
            }
            mqhdr->mqh_pid = 0;
        }
    } else if (attr->mq_curmsgs >= attr->mq_maxmsg) {
        if (mqinfo->mqi_flags & O_NONBLOCK) {
            errno = EAGAIN;
            goto err;
        }

        while (attr->mq_curmsgs >= attr->mq_maxmsg) {
            pthread_cond_wait(&mqhdr->mqh_wait, &mqhdr->mqh_lock);
        }
    }

    if ((freeindex = mqhdr->mqh_free) == 0) {
        err_dump("mq_send: curmsgs = %ld; free = 0", attr->mq_curmsgs);
    }

    nmsghdr           = (struct msg_hdr *)&mptr[freeindex];
    nmsghdr->msg_prio = prio;
    nmsghdr->msg_len  = len;
    memcpy(nmsghdr + 1, ptr, len);
    mqhdr->mqh_free = nmsghdr->msg_next;

    index   = mqhdr->mqh_head;
    pmsghdr = (struct msg_hdr *)&(mqhdr->mqh_head);
    while (index != 0) {
        msghdr = (struct msg_hdr *)&mptr[index];
        if (prio > msghdr->msg_prio) {
            nmsghdr->msg_next = index;
            pmsghdr->msg_next = freeindex;
            break;
        }
        index   = msghdr->msg_next;
        pmsghdr = msghdr;
    }

    if (index == 0) {
        pmsghdr->msg_next = freeindex;
        nmsghdr->msg_next = 0;
    }

    if (attr->mq_curmsgs == 0) {
        pthread_cond_signal(&mqhdr->mqh_wait);
    }

    attr->mq_curmsgs++;

    pthread_mutex_unlock(&mqhdr->mqh_lock);
    return (0);
err:
    pthread_mutex_unlock(&mqhdr->mqh_lock);
    return (-1);
}

int
mq_setattr(mqd_t mqd, const struct mq_attr *mqstat, struct mq_attr *omqstat)
{
    int n;
    struct mq_hdr *mqhdr;
    struct mq_attr *attr;
    struct mq_info *mqinfo;

    mqinfo = mqd;
    if (mqinfo->mqi_magic != MQI_MAGIC) {
        errno = EBADF;
        return (-1);
    }

    mqhdr = mqinfo->mqi_hdr;
    attr  = &mqhdr->mqh_attr;
    if ((n = pthread_mutex_lock(&mqhdr->mqh_lock)) != 0) {
        errno = n;
        return (-1);
    }

    if (omqstat != NULL) {
        omqstat->mq_flags   = mqinfo->mqi_flags;
        omqstat->mq_maxmsg  = attr->mq_maxmsg;
        omqstat->mq_msgsize = attr->mq_msgsize;
        omqstat->mq_curmsgs = attr->mq_curmsgs;
    }

    if (mqstat->mq_flags & O_NONBLOCK)
        mqinfo->mqi_flags |= O_NONBLOCK;
    else
        mqinfo->mqi_flags &= ~O_NONBLOCK;

    pthread_mutex_unlock(&mqhdr->mqh_lock);
    return (0);
}

int
mq_unlink(const char *pathname)
{
    if (unlink(pathname) == -1) {
        return (-1);
    }
    return (0);
}
