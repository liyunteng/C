/*
 * client_lib.cpp -- zmq client
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/07/14 19:25:49
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
#include "so_log.h"
#include "functions.hpp"

#define ENDPOINT "tcp://localhost:1234"

#define CHECK()                                 \
    if (!inited) {                              \
        iHiLog::error("Functions not init.");   \
        return ERR_NOT_INIT;                    \
    }

int Functions::getReturn()
{
    int ret;
    int buf = 0;
    ret = zmq_recv(request, &buf, sizeof(buf), 0);
    if (ret == -1) {
        iHiLog::error("zmq_recv failed: %s.", zmq_strerror(errno));
        return errno;
    } else {
        iHiLog::debug("zmq recv: %d\n", buf);
    }
    return buf;
}

int Functions::avSetEncIFramePeriod(const char *period)
{
    CHECK();

    int ret;
    if (strlen(period) >= VIDEO_ENC_PARAM_LEN) {
        iHiLog::error("avSetEncIFramePeriod args invalid.");
        return ERR_ARG_INVALID;
    }

    ParamAVSetEncIFramePeriod per;
    per.h.cmd = CMD_AVSETENC_IFRAMEPERIOD;
    memcpy(per.period, period, sizeof(per.period));
    ret = zmq_send(request, &per, sizeof(per), 0);
    if (ret == -1) {
        iHiLog::error("zmq_send failed: %s.", zmq_strerror(errno));
        return errno;
    } else {
        iHiLog::debug("zmq send: avSetEncIFramePeriod, size: %lu", sizeof(per));
    }
    return getReturn();
}

int Functions::avSetEncBitsRate(const char *bits)
{
    CHECK();

    int ret;
    if (strlen(bits) >= VIDEO_ENC_PARAM_LEN){
	    iHiLog::error("avSetEncBitsRate args invalid.");
        return ERR_ARG_INVALID;
    }

    ParamAVSetEncBitsRate bitsrate;
    bitsrate.h.cmd = CMD_AVSETENC_BITSRATE;
    memcpy(bitsrate.bitsRate, bits, sizeof(bitsrate.bitsRate));
    ret = zmq_send(request, &bitsrate, sizeof(bitsrate), 0);
    if (ret == -1) {
        iHiLog::error("zmq_send failed: %s.", zmq_strerror(errno));
        return errno;
    } else {
        iHiLog::debug("zmq send: avSetEncBitsRate, size: %lu", sizeof(bitsrate));
    }

    return getReturn();
}

int Functions::avSetEncSolution(const char *width, const char *height)
{
    CHECK();

    int ret;
    if (strlen(width) >= VIDEO_ENC_PARAM_LEN || strlen(height) >= VIDEO_ENC_PARAM_LEN) {
        iHiLog::error("avSetEncSolution args invalid.");
        return ERR_ARG_INVALID;
    }

    ParamAVSetEncSolution solution;
    solution.h.cmd =  CMD_AVSETENC_SOLUTION;
    memcpy(solution.width, width, sizeof(solution.width));
    memcpy(solution.height, height, sizeof(solution.height));
    ret = zmq_send(request, &solution, sizeof(solution), 0);
    if (ret == -1) {
        iHiLog::error("zmq_send failed: %s.", zmq_strerror(errno));
        return errno;
    } else {
        iHiLog::debug("zmq send: avSetEncSolution, size: %lu", sizeof(solution));
    }

    return getReturn();
}

int Functions::avSetLayout(picture pic[4])
{
    CHECK();

    int ret;
    ParamSetAVLayout layout;
    layout.h.cmd = CMD_AVSETLAYOUT;
    memcpy(layout.pictures, pic, sizeof(layout.pictures));

    ret = zmq_send(request, &layout, sizeof(layout), 0);
    if (ret == -1) {
        iHiLog::error("zmq_send failed: %s.", zmq_strerror(errno));
        return errno;
    } else {
        iHiLog::debug("zmq send: avSetLayout, size: %lu", sizeof(layout));
    }

    return getReturn();
}

int Functions::avSetLayoutPip(int stream1, int stream2)
{
	picture layout[4] = {
            {0, 0, 1920, 1080, stream1, 1},
            {1280, 720, 640, 360, stream2, 1},
            {0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0}
        };

    return avSetLayout(layout);
}
int Functions::avSetLayout3(int stream1, int stream2, int stream3)
{
        picture layout[4] = {
            {480, 0, 950, 530, stream1, 1},
            {0, 540, 950, 530, stream2, 1},
            {960, 540, 950, 530, stream3, 0},
            {0, 0, 0, 0, 0, 0}
    };

    return avSetLayout(layout);
}

int Functions::avSetLayout4(int stream1, int stream2, int stream3, int stream4)
{
        picture layout[4] = {
            {0, 0, 950, 530, stream1, 1},
            {960, 0, 950, 530, stream2, 1},
            {0, 540, 950, 530, stream3, 0},
            {960, 540, 950, 530, stream4, 0}
    };

    return avSetLayout(layout);
}

int Functions::avStart()
{
    CHECK()

    int ret;
    ParamAVStart av_start = {CMD_AVSTART};
    ret = zmq_send(request, &av_start, sizeof(av_start), 0);
    if (ret == -1) {
        iHiLog::error("zmq_send failed: %s.", zmq_strerror(errno));
        return errno;
    } else {
        iHiLog::debug("zmq send: avStart, size: %lu", sizeof(av_start));
    }

    return getReturn();
}

int Functions::init()
{
    int ret;
    context = zmq_ctx_new();
    if (context == NULL) {
        iHiLog::fatal("zmq_ctx_new failed: %s.", zmq_strerror(errno));
        return errno;
    }

    request = zmq_socket(context, ZMQ_REQ);
    if (request == NULL) {
        iHiLog::fatal("zmq_socket failed: %s.", zmq_strerror(errno));
        return errno;
    }

    ret = zmq_connect(request, ENDPOINT);
    if (ret == -1) {
        iHiLog::fatal("zmq_connect failed: %s.", zmq_strerror(errno));
        return errno;
    }

    iHiLog::debug("zmq init successed.");
    inited = true;
    return 0;
}

Functions::Functions():
    context(NULL), request(NULL), inited(false)
{

}

Functions::~Functions()
{
    inited = false;
    zmq_close(request);
    zmq_ctx_destroy(context);
}
