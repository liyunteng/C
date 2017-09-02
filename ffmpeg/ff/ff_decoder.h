/*
 * Filename: ff_decoder.h
 * Description: ff decoder
 *
 * Copyright (C) 2017 StreamOcean
 *
 * Author: liyunteng <liyunteng@streamocean.com>
 * License: StreamOcean
 * Last-Updated: 2017/09/03 04:24:27
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

typedef enum {
    TYPE_VIDEO_STREAM,
    TYPE_AUDIO_STREAM,
    TYPE_VIDEO_VOD,
    TYPE_AUDIO_VOD,
    TYPE_PICTURE,
    TYPE_SUBTITLE,
    TYPE_UNKNOWN,
} in_t;

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
    in_t type;
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
    in_t type;

    AVFormatContext *fctx;
    AVCodecContext *cctx;
    AVCodec *codec;
    AVFrame *frame;
    AVPacket *packet;

    AVFilterGraph *gctx;
    struct filter *src;
    struct filter *sink;
    struct filter_head filters;

    int index;
    pthread_t pid;
    bool running;

    struct picture_head pics;
    FILE *fp;                   /* for debug */

    TAILQ_ENTRY(stream_in) l;
};
TAILQ_HEAD(stream_head, stream_in);

struct stream_in * create_stream_in(const char *url);

#endif
