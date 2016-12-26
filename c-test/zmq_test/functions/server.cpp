/*
 * server.c -- zmq server
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/07/14 18:45:30
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
#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include "so_log.h"
#include "av_struct.h"
#include "av_main_class.h"
#include "zlog.h"
#include "ti_media_std.h"

#include "comm.h"

#define ENDPOINT "tcp://*:1234"

VIDEO_ENC_PARA_S enc_test = {
    {"1280"},
    {"720"},
    {"1920000"},
    {"60"},
};

STREAM_LAYOUT_S layout_test[] = {
    {0, 0, 1440, 900, 0xffff, 1},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0},
};

void sig_hdl(int signum)
{
    if (signum == SIGINT) {
        printf("catch sigint.\n");
        signal(SIGINT, SIG_DFL);
    }
}

int main(int argc, char *argv[])
{
    char recv[1024];
    int ret = 0;

    iHiLog::init();
    if (signal(SIGINT, sig_hdl) == SIG_ERR) {
        iHiLog::warn("signal SIGINT failed.");
    }

    void *context = zmq_ctx_new();
    if (context == NULL) {
        iHiLog::fatal("zmq_ctx_new failed: %s.", zmq_strerror(errno));
        return errno;
    }

    void *responder = zmq_socket(context, ZMQ_REP);
    if (responder == NULL) {
        iHiLog::fatal("zmq_socket failed: %s.", zmq_strerror(errno));
        return errno;
    }

    int rc = zmq_bind(responder, ENDPOINT);
    if (rc != 0) {
        iHiLog::fatal("zmq_bind failed: %s.", zmq_strerror(errno));
        return errno;
    }

    iHiLog::debug("%s started successed.", argv[0]);;


    avMainClass *avMain = new avMainClass(&enc_test, layout_test);
    while(1) {
        rc = zmq_recv(responder, recv, sizeof(recv), 0);
        if (rc == -1) {
            iHiLog::warn("zmq_recv failed: %s.", zmq_strerror(errno));
            continue;
        }

        Int32 cmd = ((Header *)recv)->cmd;
        switch (cmd) {
        case CMD_AVSETLAYOUT:
            {
                iHiLog::debug("updateLayout size: %lu", sizeof(ParamSetAVLayout));
                ParamSetAVLayout *layout = (ParamSetAVLayout *)recv;
                for (int i = 0; i < 4; i++) {
                    iHiLog::debug("x: %4u, y: %4u, width: %4u, height: %4u, stream_id: %4d vol: %2d",
                                  layout->pictures[i].startX, layout->pictures[i].startY, layout->pictures[i].width,
                                  layout->pictures[i].height, layout->pictures[i].streamId, layout->pictures[i].volume);
                }
                iHiLog::debug("\n");
               ret = avMain->setLayoutPara((STREAM_LAYOUT_S *)layout->pictures);
            }
            break;

        case CMD_AVSTART:
            {
                iHiLog::debug("av_start size: %lu\n", sizeof(ParamAVStart));
                ret = avMain->playMainStart();
            }
            break;
        case CMD_AVSETENC_SOLUTION:
            {
                iHiLog::debug("setEncSolution size: %lu", sizeof(ParamAVSetEncSolution));
                ParamAVSetEncSolution *solution = (ParamAVSetEncSolution *)recv;
                iHiLog::debug("width: %s, height: %s\n", (char *)solution->width, (char *)solution->height);
                ret = avMain->setEncSolution((Int8 *)solution->width, (Int8 *)solution->height);
            }
            break;

        case CMD_AVSETENC_BITSRATE:
            {
                iHiLog::debug("setEncBitsRate size: %lu", sizeof(ParamAVSetEncBitsRate));
                ParamAVSetEncBitsRate *bits = (ParamAVSetEncBitsRate *)recv;
                iHiLog::debug("bitsrate: %s\n", (char *)bits->bitsRate);

		ret = avMain->setEncBitsRate((Int8 *)bits->bitsRate);
            }
            break;

        case CMD_AVSETENC_IFRAMEPERIOD:
            {
                iHiLog::debug("setEncIFramePeriod size: %lu", sizeof(ParamAVSetEncIFramePeriod));
                ParamAVSetEncIFramePeriod *period = (ParamAVSetEncIFramePeriod *)recv;
                iHiLog::debug("period: %s\n", (char *)period->period);

                ret = avMain->setEncIFramePeriod((Int8 *)period->period);
            }
            break;
        case CMD_AVINIT:
            {
                iHiLog::debug("setAVInit size: %lu", sizeof(ParamAVInit));
                ParamAVInit *av_init = (ParamAVInit *)recv;
                iHiLog::debug("width: %s height: %s bitRate: %s period: %s\n",
                              av_init->param.encWidth, av_init->param.encHeight, av_init->param.encBitRate, av_init->param.IFramePeriod);
            }
            break;
        case CMD_TEST:
            {
                iHiLog::debug("test size: %lu", sizeof(ParamTest));
                ParamTest *test = (ParamTest *)recv;
                iHiLog::debug("%s\n", test->test);
            }
            break;
        default:
            break;

        }
        rc = zmq_send(responder, &ret, sizeof(ret), 0);
        if (rc == -1) {
            iHiLog::warn("zmq_send failed: %s.",zmq_strerror(errno) );
            continue;
        }
    }

    zmq_close(responder);
    zmq_ctx_destroy(context);

    return 0;
}
