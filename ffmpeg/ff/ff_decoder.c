/*
 * Filename: ff_decoder.c
 * Description: ff decoder
 *
 * Copyright (C) 2017 StreamOcean
 *
 * Author: liyunteng <liyunteng@streamocean.com>
 * License: StreamOcean
 * Last-Updated: 2017/09/03 04:20:41
 */
#include <string.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "ff_decoder.h"

static int
open_input(struct stream_in *in)
{
    if (in == NULL) {
        return -1;
    }

    int rc = avformat_open_input(&in->fctx, in->url, NULL, NULL);
    if (rc != 0) {
        ERROR("%s open input failed: %s", in->url, av_err2str(rc));
        return -1;
    }

    rc = avformat_find_stream_info(in->fctx, NULL);
    if (rc != 0) {
        ERROR("%s find stream info failed: %s", in->url, av_err2str(rc));
        return -1;
    }

    if (in->type == TYPE_VIDEO_STREAM) {
        rc = av_find_best_stream(in->fctx, AVMEDIA_TYPE_VIDEO ,-1, -1, &in->codec, 0);
        if (rc < 0) {
            ERROR("%s cant find a video stream: %s", in->url, av_err2str(rc));
            return -1;
        }
        in->index = rc;
    } else if (in->type == TYPE_AUDIO_STREAM) {
        rc = av_find_best_stream(in->fctx, AVMEDIA_TYPE_AUDIO, -1, -1, &in->codec, 0);
        if (rc < 0) {
            ERROR("%s can't find a audio stream: %s", in->url, av_err2str(rc));
            return -1;
        }
        in->index = rc;
    } else {
        return -1;
    }

    if (in->index == -1) {
        ERROR("%s find stream failed", in->url);
        return -1;
    }

    INFO("%s codec: %s", in->url, in->codec->long_name);
    in->cctx = avcodec_alloc_context3(in->codec);
    if (in->cctx == NULL) {
        ERROR("%s alloc codec ctx failed", in->url);
        return -1;
    }

    rc = avcodec_parameters_to_context(in->cctx, in->fctx->streams[in->index]->codecpar);
    av_opt_set_int(in->cctx, "refcounted_frames", 1, 0);

    /*
     * if (in->type == TYPE_VIDEO_STREAM) {
     *     av_codec_set_pkt_timebase(in->cctx, in->fctx->streams[in->index]->time_base);
     * }
     */

    rc = avcodec_open2(in->cctx, in->codec, NULL);
    if (rc != 0) {
        ERROR("%s open codec failed: %s", in->url, av_err2str(rc));
        return -1;
    }
    return 0;
}

static struct filter *
create_filter(const char *filter_name, const char *name, const char *cmd, struct stream_in *in)
{
    struct filter *f = (struct filter *)calloc(1, sizeof(struct filter));
    if (f == NULL) {
        return NULL;
    }
    strncpy(f->fname, filter_name, sizeof(f->fname)-1);
    f->f = avfilter_get_by_name(f->fname);
    strncpy(f->cmd, cmd, CMD_MAX_LEN-1);
    strncpy(f->name, name, sizeof(f->name)-1);
    int rc = avfilter_graph_create_filter(&f->ctx,f->f,name,f->cmd, NULL, in->gctx);
    if (rc < 0) {
        ERROR("%s add filter [%s:%s] failed: %s", in->url, f->fname, f->name, av_err2str(rc));
        free(f);
        return NULL;
    }
    INFO("%s add filter [%s:%s] cmd: %s", in->url, f->fname, f->name, f->cmd);
    return f;
}

void *
in_run(void *arg)
{
    struct stream_in *in = (struct stream_in *)arg;
    if (in == NULL) {
        ERROR("arg ivalid");
        return (void *)-1;
    }

    int rc;
 
/* for debug */
    uint64_t a_out_channel_layout = AV_CH_LAYOUT_STEREO;
    int a_out_nb_samples = 0;
    enum AVSampleFormat a_out_sample_fmt = AV_SAMPLE_FMT_S16;
    int a_out_sample_rate = 44100;
    int a_out_channels = av_get_channel_layout_nb_channels(a_out_channel_layout);
    int a_out_buffer_size = av_samples_get_buffer_size(NULL,a_out_channels, a_out_nb_samples, a_out_sample_fmt, 1);
    int64_t a_in_channel_layout = av_get_default_channel_layout(in->cctx->channels);
    uint8_t *a_out_buf = NULL;
    struct SwrContext *a_convert_ctx = NULL;
    if (in->type == TYPE_AUDIO_STREAM) {
#define MAX_AUDIO_FRAME_SIZE 192000
        a_out_buf = (uint8_t *)av_malloc(MAX_AUDIO_FRAME_SIZE);
        a_convert_ctx = swr_alloc();
        a_convert_ctx = swr_alloc_set_opts(a_convert_ctx, a_out_channel_layout, a_out_sample_fmt, a_out_sample_rate,
                                           a_in_channel_layout, in->cctx->sample_fmt,
                                           in->cctx->sample_rate, 0, NULL);
        swr_init(a_convert_ctx);
    }


    while (1) {
        if (in->running == false) {
            struct timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = 500 * 1000;
            select(0, NULL, NULL, NULL, &tv);
            avcodec_flush_buffers(in->cctx);
            av_packet_unref(in->packet);
            av_frame_unref(in->frame);
            continue;
        }
        if (av_read_frame(in->fctx, in->packet) == 0) {
            if (in->packet->stream_index == in->index) {
                rc = avcodec_send_packet(in->cctx, in->packet);
                if (rc != 0) {
                    WARNING("%s send packet failed: %s", in->url, av_err2str(rc));
                    continue;
                }
                while (rc >= 0) {
                    rc = avcodec_receive_frame(in->cctx, in->frame);
                    if (rc != 0) {
                        if (rc == AVERROR(EAGAIN)) {
                            struct timeval tv;
                            tv.tv_sec = 0;
                            tv.tv_usec = 10 * 1000;
                            select(0, NULL, NULL, NULL, &tv);
                            break;
                        } else {
                            WARNING("%s receive frame failed: %s", in->url, av_err2str(rc));
                            continue;
                        }
                    }

#if 0
                    /* for debug */
                    if (in->fp) {
                        if (in->type == TYPE_VIDEO_STREAM) {
                            int size = in->cctx->width * in->cctx->height;
                            fwrite(in->frame->data[0], 1, size, in->fp);
                            fwrite(in->frame->data[1], 1, size/4, in->fp);
                            fwrite(in->frame->data[2], 1, size/4, in->fp);
                        } else if (in->type == TYPE_AUDIO_STREAM) {
                            swr_convert(a_convert_ctx, &a_out_buf, MAX_AUDIO_FRAME_SIZE,
                                        (const uint8_t **)in->frame->data, in->frame->nb_samples);
                            fwrite(a_out_buf, 1, a_out_buffer_size, in->fp);
                        }
                    }
#endif
                    in->frame->pts = in->frame->best_effort_timestamp;
                    rc = av_buffersrc_add_frame_flags(in->src->ctx,in->frame, 0);
                    if (rc < 0) {
                        WARNING("%s filter buffersrc add frame failed: %s", in->url, av_err2str(rc));
                        break;
                    }

                    av_frame_unref(in->frame);
                }
            }
            av_packet_unref(in->packet);
        }
    }


    return (void *)0;
}


struct stream_in *
create_stream_in(const char *url)
{
    int rc;
    struct stream_in *in = NULL;
    in = (struct stream_in *)calloc(1, sizeof(struct stream_in));
    if (in == NULL) {
        WARNING("calloc failed: %s", strerror(errno));
        return NULL;
    }

    strncpy(in->url, url, URL_MAX_LEN-1);
    INFO("url: %s", in->url);
    in->pid = -1;
    in->index = -1;

    TAILQ_INIT(&in->pics);
    struct picture *p = (struct picture *)calloc(1, sizeof(struct picture));
    strncpy(p->url, "/home/lyt/Pictures", URL_MAX_LEN-1);
    p->type = TYPE_PICTURE;
    p->x = 10;
    p->y = 20;
    p->w = 640;
    p->h = 480;
    TAILQ_INSERT_TAIL(&in->pics, p, l);

    in->gctx = avfilter_graph_alloc();
    TAILQ_INIT(&in->filters);

    in->type = TYPE_VIDEO_STREAM;
    /*
     * if (in->type == TYPE_VIDEO_STREAM) {
     *     in->fp = fopen("abc.yuv", "wb+");
     * } else if (in->type == TYPE_AUDIO_STREAM) {
     *     in->fp = fopen("abc.pcm", "wb+");
     * }
     */
    in->running = false;

    if (open_input(in) < 0) {
        goto clean;
    }
    av_dump_format(in->fctx, 0,in->url, 0);
    in->frame = av_frame_alloc();
    in->packet = av_packet_alloc();


    char cmd[CMD_MAX_LEN];
    /* buffer filter */
    snprintf(cmd, CMD_MAX_LEN,
             "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
             in->cctx->width, in->cctx->height, in->cctx->pix_fmt,
             in->fctx->streams[in->index]->time_base.num,
             in->fctx->streams[in->index]->time_base.den,
             in->cctx->sample_aspect_ratio.num,
             in->cctx->sample_aspect_ratio.den);
    in->src = create_filter("buffer", "in", cmd, in);
    if (in->src == NULL) {
        ERROR("%s create buffer filter failed", in->url);
        goto clean;
    }
    /* buffersink filter */
    in->sink = (struct filter *)calloc(1, sizeof(struct filter));
    strcpy(in->sink->name, "buffersink");
    in->sink->f = avfilter_get_by_name(in->sink->name);
    enum AVPixelFormat pix_fmts[] = {AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE};
    rc = avfilter_graph_create_filter(&in->sink->ctx, in->sink->f, "out", NULL, NULL, in->gctx);
    if (rc < 0) {
        ERROR("%s create filter %s failed: %s", in->url, in->sink->name, av_err2str(rc));
    }
    av_opt_set_int_list(in->sink->ctx, "pix_fmts", pix_fmts, AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
    if (rc < 0) {
        ERROR("%s set output pixel format failed: %s", in->url, av_err2str(rc));
    }


    snprintf(cmd, CMD_MAX_LEN, "/home/lyt/Pictures/a.gif");
    struct filter *f2 = create_filter("movie", "m", cmd, in);
    if (f2 != NULL) {
        TAILQ_INSERT_TAIL(&in->filters, f2, l);
    }

    snprintf(cmd, CMD_MAX_LEN, "10:10");
    struct filter *f3 = create_filter("overlay", "o", cmd, in);
    if (f3 != NULL) {
        TAILQ_INSERT_TAIL(&in->filters, f3, l);
    }

    snprintf(cmd, CMD_MAX_LEN, "w=320:h=240");
    struct filter *f4 = create_filter("scale", "s", cmd, in);
    if (f4 != NULL) {
        TAILQ_INSERT_TAIL(&in->filters, f4, l);
    }

    snprintf(cmd, CMD_MAX_LEN, "/home/lyt/Pictures/b.jpg");
    struct filter *f6 = create_filter("movie", "p", cmd, in);
    if (f6 != NULL) {
        TAILQ_INSERT_TAIL(&in->filters, f6, l);
    }

    snprintf(cmd, CMD_MAX_LEN, "500:500");
    struct filter *f7 = create_filter("overlay", "o1", cmd, in);
    if (f7 != NULL) {
        TAILQ_INSERT_TAIL(&in->filters, f7, l);
    }

    snprintf(cmd, CMD_MAX_LEN, "w=320:h=240");
    struct filter *f5 = create_filter("scale", "i", "", in);
    if (f5 != NULL) {
        TAILQ_INSERT_TAIL(&in->filters, f5, l);
    }



    avfilter_link(in->src->ctx, 0, f3->ctx, 0);
    avfilter_link(f2->ctx, 0, f4->ctx, 0);
    avfilter_link(f4->ctx, 0, f3->ctx, 1);
    avfilter_link(f3->ctx, 0, f7->ctx, 0);
    avfilter_link(f6->ctx, 0, f5->ctx, 0);
    avfilter_link(f5->ctx, 0, f7->ctx, 1);
    avfilter_link(f7->ctx, 0, in->sink->ctx, 0);
    rc = avfilter_graph_config(in->gctx, NULL);
    if (rc < 0) {
        ERROR("av filter graph config failed: %s", av_err2str(rc));
        goto clean;
    }

    if (pthread_create(&in->pid, NULL, in_run, (void *)in) != 0) {
        ERROR("create thread failed: %s", strerror(errno));
        free(in);
        return NULL;
    }

    return in;

  clean:
    if (in->codec) {
        in->codec = NULL;
    }
    if (in->cctx) {
        avcodec_free_context(&in->cctx);
        in->cctx = NULL;
    }
    if (in->fctx) {
        avformat_close_input(&in->fctx);
        in->fctx = NULL;
    }
    if (in->frame) {
        av_frame_free(&in->frame);
        in->frame = NULL;
    }
    if (in->packet) {
        av_packet_free(&in->packet);
        in->packet = NULL;
    }
    if (in->src) {
        avfilter_free(in->src->ctx);
        free(in->src);
        in->src = NULL;
    }
    if (in->sink) {
        avfilter_free(in->sink->ctx);
        free(in->sink);
        in->sink = NULL;
    }
    if (in->gctx) {
        avfilter_graph_free(&in->gctx);
        in->gctx = NULL;
    }
    return NULL;
}
