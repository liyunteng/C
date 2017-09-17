/*
 * Description: ff encoder
 *
 * Copyright (C) 2017 StreamOcean
 * Last-Updated: <2017/09/17 06:09:33 liyunteng>
 */
#ifndef FF_ENCODER_H
#define FF_ENCODER_H

#include "ff_priv.h"
#include "ff_decoder.h"

struct stream_out {
    AVFormatContext *fctx;
    AVCodecContext **cctx;

    AVFilterGraph *gctx;
    struct filter *src;
    struct filter *sink;
    struct filter *asrc;
    struct filter *asink;

    pthread_t pid;
    pthread_t a_pid;

    FILE *fp;                   /* for debug */
    FILE *afp;
    uint64_t pts;
    uint64_t apts;

    struct stream_in *current_video;
    struct stream_in *current_audio;
    struct stream_head ss;
};

struct stream_out * create_stream_out(struct stream_head ss);
#endif
