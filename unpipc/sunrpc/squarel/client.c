/*
 * client.c--
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

int
main(int argc, char *argv[])
{
    CLIENT *cl;
    square_in in;
    square_out out;
    square_out *outp;

    if (argc != 3)
        err_quit("usage: client <hostname> <integer-value>\n");

    cl = clnt_create(argv[1], SQUARE_PROG, SQUARE_VERS, "tcp");
    if (cl == NULL)
        err_sys("clnt_create error: ");

    in.argl = atol(argv[2]);
    if ((outp = squareproc_1(&in, cl)) == NULL)
        err_quit("%s", clnt_sperror(cl, argv[1]));

    printf("#1 result: %ld\n", outp->resl);

    cl = clnt_create(argv[1], SQUARE_PROG, SQUARE_VERS2, "tcp");
    if (cl == NULL)
        err_sys("clnt_create error: ");

    in.argl = atol(argv[2]);
    if ((outp = squareproc_2(&in, cl)) == NULL)
        err_quit("%s", clnt_sperror(cl, argv[1]));
    printf("#2 result: %ld\n", outp->resl);
    /*
     * cl = clnt_create(argv[1], SQUARE_PROG, SQUARE_VERS3, "tcp");
     * if (cl == NULL)
     *      err_sys("clnt_create error: ");
     *
     * in.argl = atol(argv[2]);
     * if (squareproc_3(&in, &out, cl) != RPC_SUCCESS)
     *      err_quit("%s", clnt_sperror(cl, argv[1]));
     * printf("#3 result: %ld\n", out.resl);
     */

    return 0;
}
