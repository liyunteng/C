/*
 * Description: ff decoder
 *
 * Copyright (C) 2017 StreamOcean
 * Last-Updated: <2017/09/19 10:20:47 liyunteng>
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

enum stream_in_stat {
    E_INIT,
    E_FINDING_FRAME,
    E_RUNNING,
    E_WAITING_OTHER,
    E_QUIT,
};
struct stream_out;
struct stream_in {
    struct picture_head pics;

    struct stream_out *out;
    struct stream_in *pre;
    struct ff_ctx *ff;
    struct filter_head filters;

    pthread_t pid;
    bool must_i_frame;

    bool use_audio;
    bool use_video;

    enum stream_in_stat stat;

    TAILQ_ENTRY(stream_in) l;
};
TAILQ_HEAD(stream_head, stream_in);

void print_packet(const AVFormatContext *fmt_ctx, const AVPacket *pkt, const char *tag);
struct stream_in * create_stream_in(const char *url, struct stream_out *out, bool video, bool audio);
void destroy_stream_in(struct stream_in *in);

#endif
