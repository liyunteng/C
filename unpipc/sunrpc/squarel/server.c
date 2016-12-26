/*
 * server.c--
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

#include "../../unpipc.h"
#include "square.h"
#include <pthread.h>



square_out *squareproc_1_svc(square_in * inp, struct svc_req *rqstp)
{
    static square_out out;
    out.resl = inp->argl * inp->argl;
    return (&out);
}


square_out *squareproc_2_svc(square_in * inp, struct svc_req * rqstp)
{
    static square_out out;

    printf("thread %ld started, arg = %ld\n",
	   (long) pthread_self(), inp->argl);
    sleep(5);
    out.resl = inp->argl * inp->argl;
    printf("thread %ld done\n", (long) pthread_self());
    return (&out);
}


/*
 * bool_t squareproc_3_svc(square_in *inp, square_out *outp, struct svc_req *rqstp)
 * {
 *	printf("thread %ld started ,arg = %ld\n",
 *	       (long)pthread_self(), inp->argl);
 *	sleep(5);
 *	outp->resl = inp->argl * inp->argl;
 *	printf("thread %ld done\n", (long)pthread_self());
 *	return(TRUE);
 * }
 *
 * int square_prog_3_freeresult(SVCXPRT *transp, xdrproc_t xdr_result,
 *			     caddr_t result)
 * {
 *	xdr_free(xdr_result, result);
 *	return (1);
 * }
 */
