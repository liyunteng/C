/*
 * Filename: ff_decoder.h
 * Description: ff decoder
 *
 * Copyright (C) 2017 StreamOcean
 *
 * Author: liyunteng <liyunteng@streamocean.com>
 * License: StreamOcean
 * Last-Updated: 2017/09/03 19:07:16
 */
#ifndef FF_DECODER_H
#define FF_DECODER_H

#include <stdint.h>
#include <pthread.h>
#include <stdbool.h>

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/avfiltergraph.h>
#include <libavfilter/buffersrc.h>
#include <libavfilter/buffersink.h>
#include <libavutil/pixfmt.h>
#include <libavutil/opt.h>

#include <sys/queue.h>

#include "ff_priv.h"

struct filter {
    AVFilterContext *ctx;
    AVFilter *f;
    char cmd[CMD_MAX_LEN];
    char name[32];
    char fname[32];             /* filter name */
    TAILQ_ENTRY(filter) l;
};
TAILQ_HEAD(filter_head, filter);

struct picture {
    char url[URL_MAX_LEN];
    int x;
    int y;
    int w;
    int h;
    struct filter *f;
    TAILQ_ENTRY(picture) l;
};
TAILQ_HEAD(picture_head, picture);


struct stream_in {
    char url[URL_MAX_LEN];

    AVFormatContext *fctx;
    AVCodecContext **cctx;

    AVFilterGraph *gctx;
    struct filter *src;
    struct filter *sink;
    struct filter_head filters;

    struct filter *asrc;
    struct filter *asink;

    bool running;
    pthread_t pid;

    struct picture_head pics;
    FILE *fp;                   /* for debug */
    FILE *afp;                  /* for debug */

    TAILQ_ENTRY(stream_in) l;
};
TAILQ_HEAD(stream_head, stream_in);

struct stream_in * create_stream_in(const char *url);
int start(struct stream_in *s);
int stop(struct stream_in *s);

#endif
