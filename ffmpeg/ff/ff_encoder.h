/*
 * Filename: ff_encoder.h
 * Description: ff encoder
 *
 * Copyright (C) 2017 StreamOcean
 *
 * Author: liyunteng <liyunteng@streamocean.com>
 * License: StreamOcean
 * Last-Updated: 2017/09/03 18:47:58
 */
#ifndef FF_ENCODER_H
#define FF_ENCODER_H

#include "ff_priv.h"
#include "ff_decoder.h"

struct stream_out {
    AVFormatContext *fctx;
    AVCodecContext **cctx;

    AVFilterGraph *gctx;
    pthread_t pid;

    FILE *fp;                   /* for debug */
    uint64_t pts;
    uint64_t apts;

    struct stream_in *current;
    struct stream_head ss;
};

struct stream_out * create_stream_out(struct stream_head ss);
#endif
