/*
 * pthread_rwlock_1.c--
 *
 * Copyright (C) 2014,2015,  <li_yunteng@163.com>
 * Auther: liyunteng
 * License: GPL
 * Update time:  2014/11/14 17:20:10
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

#include <stdlib.h>
#include <pthread.h>

struct job {
    struct job *j_next;
    struct job *j_prev;
    pthread_t j_id;		/* tells which thread handles this job */

    /* ... more stuff here ... */
};

struct queue {
    struct job *q_head;
    struct job *q_tail;
    pthread_rwlock_t q_lock;
};

/*
 * Initialize a queue.
 */
int queue_init(struct queue *qp)
{
    int err;

    qp->q_head = NULL;
    qp->q_tail = NULL;
    err = pthread_rwlock_init(&qp->q_lock, NULL);
    if (err != 0)
	return (err);
    /* ... continue Initializetion ... */

    return (0);
}

/*
 * Insert a job at the head of the queue.
 */

void job_insert(struct queue *qp, struct job *jp)
{
    pthread_rwlock_wrlock(&qp->q_lock);
    jp->j_next = qp->q_head;
    jp->j_prev = NULL;
    if (qp->q_head != NULL) {
	qp->q_head->j_prev = jp;
    } else {
	qp->q_tail = jp;
    }
    qp->q_head = jp;
    pthread_rwlock_unlock(&qp->q_lock);
}

/*
 * Append a job on the tail of the queue.
 */
void job_append(struct queue *qp, struct job *jp)
{
    pthread_rwlock_wrlock(&qp->q_lock);
    jp->j_next = NULL;
    jp->j_prev = qp->q_tail;
    if (qp->q_tail != NULL) {
	qp->q_tail->j_next = jp;
    } else {
	qp->q_head = jp;
    }
    qp->q_tail = jp;
    pthread_rwlock_unlock(&qp->q_lock);
}

/*
 * Remove the given job from queue.
 */
void job_remove(struct queue *qp, struct job *jp)
{
    pthread_rwlock_wrlock(&qp->q_lock);
    if (jp == qp->q_head) {
	qp->q_head = jp->j_next;
	if (qp->q_tail == jp)
	    qp->q_tail = NULL;
    } else if (jp == qp->q_tail) {
	qp->q_tail = jp->j_prev;
	if (qp->q_head == jp)
	    qp->q_head = NULL;
    } else {

	jp->j_prev->j_next = jp->j_next;
	jp->j_next->j_prev = jp->j_prev;
    }
    pthread_rwlock_unlock(&qp->q_lock);
}

/*
 * Find a job for the given thread ID
 */
struct job *job_find(struct queue *qp, pthread_t id)
{
    struct job *jp;

    /* 由于读写锁的实现，一般对读占用锁的个数加以限制，
     * 读写锁在加读锁的时候可能会报错 */
    if (pthread_rwlock_rdlock(&qp->q_lock) != 0) {
	return (NULL);
    }
    for (jp = qp->q_head; jp != NULL; jp = jp->j_next) {
	if (pthread_equal(jp->j_id, id))
	    break;
    }
    pthread_rwlock_unlock(&qp->q_lock);
    return (jp);
}
