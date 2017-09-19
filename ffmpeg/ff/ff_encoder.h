/*
 * Description: ff encoder
 *
 * Copyright (C) 2017 StreamOcean
 * Last-Updated: <2017/09/18 18:51:19 liyunteng>
 */
#ifndef FF_ENCODER_H
#define FF_ENCODER_H
#include "ff_decoder.h"
#include "ff_priv.h"

struct stream_out {
    struct ff_ctx *ff;
    pthread_t pid;

    /* struct stream_head ins; */
    unsigned last_video_pts;
    pthread_mutex_t mutex;
};

struct stream_out * create_stream_out(const char *url);
int add_to_out(struct stream_in*in, struct stream_out *out);
int remove_from_out(struct stream_in*in, struct stream_out *out);
#endif
