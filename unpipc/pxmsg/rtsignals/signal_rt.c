/*
 * signal_rt.c--
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

#include <signal.h>
#include "../../unpipc.h"

typedef void Sigfunc_rt(int, siginfo_t *, void *);

Sigfunc_rt *signal_rt(int signo, Sigfunc_rt * func, sigset_t * mask)
{
    struct sigaction act, oact;

    act.sa_sigaction = func;	/* must store function addr here */
    act.sa_mask = *mask;	/* signals to block */
    act.sa_flags = SA_SIGINFO;	/* must specify this for realtime */

    if (signo == SIGALRM) {
#ifdef SA_INTERRUPT
	act.sa_flags |= SA_INTERRUPT;
#endif
    } else {
#ifdef SA_RESTART
	act.sa_flags |= SA_RESTART;
#endif
    }

    if (sigaction(signo, &act, &oact) < 0) {
	return ((Sigfunc_rt *) SIG_ERR);
    }
    return (oact.sa_sigaction);
}
