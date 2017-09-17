/*
 * Description: ff decoder
 *
 * Copyright (C) 2017 StreamOcean
 * Last-Updated: <2017/09/17 09:37:36 liyunteng>
 */

#include <string.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <libavutil/time.h>
#include "ff_decoder.h"

struct fileio {
    FILE *fp;
    int (*read_packet)(void *opaque, uint8_t *buf, int buf_size);
};

int my_read(void *userp, uint8_t *buf, int buf_size)
{
    struct fileio *op = (struct fileio *)userp;
    if (feof(op->fp)) {
        rewind(op->fp);
    }

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 23222;
    select(0, NULL, NULL, NULL, &tv);
    DEBUG("my read");
    int size = fread(buf, 1, buf_size, op->fp);
    return size;
}

static int
open_input(struct stream_in *in)
{
    if (in == NULL) {
        return -1;
    }

    int rc;
    if (in->use_audio && !in->use_video) {
        in->fctx = avformat_alloc_context();
        struct fileio *op = (struct fileio *)calloc(1, sizeof(struct fileio));
        op->fp = fopen(in->url, "rb");
        unsigned char *buffer = (unsigned char *)av_malloc(1024*2);
        AVIOContext *avio = avio_alloc_context(buffer, 1024*2, 0, op,  my_read, NULL, NULL);
        in->fctx->pb = avio;
    }

    rc = avformat_open_input(&in->fctx, in->url, NULL, NULL);
    if (rc != 0) {
        ERROR("%s open input failed: %s", in->url, av_err2str(rc));
        return rc;
    }

    rc = avformat_find_stream_info(in->fctx, NULL);
    if (rc != 0) {
        ERROR("%s find stream info failed: %s", in->url, av_err2str(rc));
        return rc;
    }

    unsigned int i;
    for (i = 0; i < in->fctx->nb_streams; i++) {
        if (in->use_video && in->video_id == -1
            && in->fctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            in->video_id = i;
            AVStream *s = in->fctx->streams[i];
            AVCodec *dec = avcodec_find_decoder(s->codecpar->codec_id);
            if (!dec) {
                ERROR("%s can't find codec for decode stream %u", in->url, i);
                return AVERROR_DECODER_NOT_FOUND;
            }
            in->vctx = avcodec_alloc_context3(dec);
            if (in->vctx == NULL) {
                ERROR("%s alloc codec context failed: %s", in->url, av_err2str(AVERROR(ENOMEM)));
                return AVERROR(ENOMEM);
            }
            rc = avcodec_parameters_to_context(in->vctx,s->codecpar);
            if (rc < 0) {
                ERROR("%s copy decoder paramerter to ctx failed :%s",
                      in->url, av_err2str(rc));
                return rc;
            }
            in->vctx->framerate = av_guess_frame_rate(in->fctx, s, NULL);
            rc = avcodec_open2(in->vctx, dec, NULL);
            if (rc < 0) {
                ERROR("%s open decoder for %d failed: %s", in->url, i,
                      av_err2str(rc));
                return rc;
            }
        }

        if (in->use_audio && in->audio_id == -1 &&
            in->fctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            in->audio_id = i;
            AVStream *s = in->fctx->streams[i];
            AVCodec *dec = avcodec_find_decoder(s->codecpar->codec_id);
            if (!dec) {
                ERROR("%s can't find codec for decode stream %u", in->url, i);
                return AVERROR_DECODER_NOT_FOUND;
            }
            in->actx = avcodec_alloc_context3(dec);
            if (in->actx == NULL) {
                ERROR("%s alloc codec context failed: %s", in->url, av_err2str(AVERROR(ENOMEM)));
                return AVERROR(ENOMEM);
            }
            rc = avcodec_parameters_to_context(in->actx,s->codecpar);
            if (rc < 0) {
                ERROR("%s copy decoder paramerter to ctx failed :%s",
                      in->url, av_err2str(rc));
                return rc;
            }
            rc = avcodec_open2(in->actx, dec, NULL);
            if (rc < 0) {
                ERROR("%s open decoder for %d failed: %s", in->url, i,
                      av_err2str(rc));
                return rc;
            }
        }
    }

    av_dump_format(in->fctx, 0, in->url, 0);
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
    const char *arg = NULL;
    if (strlen(f->cmd) != 0) {
        arg = f->cmd;
    }
    int rc = avfilter_graph_create_filter(&f->ctx,f->f,name, arg, NULL, in->gctx);
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
    AVCodecContext *ctx = NULL;

#ifndef NDEBUG
/* for debug */
    uint64_t a_out_channel_layout = AV_CH_LAYOUT_STEREO;
    int a_out_nb_samples = 0;
    enum AVSampleFormat a_out_sample_fmt = AV_SAMPLE_FMT_S16;
    int a_out_sample_rate = 44100;
    int a_out_channels = av_get_channel_layout_nb_channels(a_out_channel_layout);
    int a_out_buffer_size = av_samples_get_buffer_size(NULL,a_out_channels, a_out_nb_samples, a_out_sample_fmt, 1);
    int64_t a_in_channel_layout = av_get_default_channel_layout(in->actx->channels);
    uint8_t *a_out_buf = NULL;
    struct SwrContext *a_convert_ctx = NULL;

#define MAX_AUDIO_FRAME_SIZE 192000
    a_out_buf = (uint8_t *)av_malloc(MAX_AUDIO_FRAME_SIZE);
    a_convert_ctx = swr_alloc();
    a_convert_ctx = swr_alloc_set_opts(a_convert_ctx, a_out_channel_layout, a_out_sample_fmt, a_out_sample_rate,
                                       a_in_channel_layout, in->actx->sample_fmt,
                                       in->actx->sample_rate, 0, NULL);
    swr_init(a_convert_ctx);
#endif


    AVPacket *packet = av_packet_alloc();
    if (packet == NULL) {
        ERROR("%s alloc packet failed", in->url);
        return (void *)-1;
    }

    AVFrame *frame = av_frame_alloc();
    if (frame == NULL) {
        ERROR("%s alloc frame failed", in->url);
        return (void *)-1;
    }


    while (1) {
        unsigned int idx;
        enum AVMediaType type;
        /* packet->data = NULL; */
        /* packet->size = 0; */
        if (av_read_frame(in->fctx, packet) == 0) {
            idx = packet->stream_index;
            type = in->fctx->streams[idx]->codecpar->codec_type;
            if (in->use_video && type == AVMEDIA_TYPE_VIDEO) {
                ctx = in->vctx;
            } else if (in->use_audio && type == AVMEDIA_TYPE_AUDIO) {
                ctx = in->actx;
            } else {
                av_packet_unref(packet);
                continue;
            }
            av_packet_rescale_ts(packet,
                                 in->fctx->streams[idx]->time_base,
                                 ctx->time_base);
            rc = avcodec_send_packet(ctx, packet);
            if (rc != 0) {
                WARNING("%s send packet failed: %s", in->url, av_err2str(rc));
                av_packet_unref(packet);
                continue;
            }
            while (rc >= 0) {
                rc = avcodec_receive_frame(ctx, frame);
                if (rc != 0) {
                    if (rc == AVERROR(EAGAIN)) {
                        break;
                    } else {
                        WARNING("%s receive frame failed: %s", in->url, av_err2str(rc));
                        continue;
                    }
                }

#ifndef NDEBUG
                /* for debug */
                if (type == AVMEDIA_TYPE_VIDEO) {
                    if (in->fp) {
                        int size = ctx->width * ctx->height;
                        fwrite(frame->data[0], 1, size, in->fp);
                        fwrite(frame->data[1], 1, size/4, in->fp);
                        fwrite(frame->data[2], 1, size/4, in->fp);
                    }
                } else if (type == AVMEDIA_TYPE_AUDIO) {
                    if (in->afp) {
                        swr_convert(a_convert_ctx, &a_out_buf, MAX_AUDIO_FRAME_SIZE,
                                    (const uint8_t **)frame->data, frame->nb_samples);
                        fwrite(a_out_buf, 1, a_out_buffer_size, in->afp);
                    }
                }

#endif
                if (in->running) {
                    if (type == AVMEDIA_TYPE_VIDEO) {
                        frame->pts = frame->best_effort_timestamp;
                        rc = av_buffersrc_add_frame_flags(in->src->ctx,frame, 0);
                        if (rc < 0) {
                            WARNING("%s filter buffersrc add frame failed: %s", in->url, av_err2str(rc));
                        }
                        /* DEBUG("add video frame"); */
                    } else if (type == AVMEDIA_TYPE_AUDIO) {
                        rc = av_buffersrc_add_frame_flags(in->asrc->ctx,frame, 0);
                        if (rc < 0) {
                            WARNING("%s filter buffersrc add frame failed: %s", in->url, av_err2str(rc));
                        }
                        /* DEBUG("add audio frame"); */
                    }
                }

                av_frame_unref(frame);
            }
            av_packet_unref(packet);
        }
    }


    return (void *)0;
}

int
start(struct stream_in *stream)
{
    if (stream == NULL) {
        return -1;
    }
    /*
     * for (unsigned int i=0; i < stream->fctx->nb_streams; i++) {
     *     avcodec_send_packet(stream->cctx[i], NULL);
     *     avcodec_flush_buffers(stream->cctx[i]);
     * }
     */
    stream->running = true;

    INFO("start %s", stream->url);
    return 0;
}

int
stop(struct stream_in *stream)
{
    if (stream == NULL) {
        return -1;
    }
    stream->running = false;
    /*
     * for (unsigned int i=0; i < stream->fctx->nb_streams; i++) {
     *     avcodec_send_packet(stream->cctx[i], NULL);
     *     avcodec_flush_buffers(stream->cctx[i]);
     * }
     */
    INFO("stop %s", stream->url);
    return 0;
}

struct stream_in *
create_stream_in(const char *url, bool video, bool audio)
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
    in->fctx = NULL;
    in->actx = NULL;
    in->vctx = NULL;
    in->use_video = video;
    in->use_audio = audio;
    in->audio_id = -1;
    in->video_id = -1;

    TAILQ_INIT(&in->pics);
    struct picture *p = (struct picture *)calloc(1, sizeof(struct picture));
    strncpy(p->url, "/home/lyt/Pictures", URL_MAX_LEN-1);
    p->x = 10;
    p->y = 20;
    p->w = 640;
    p->h = 480;
    TAILQ_INSERT_TAIL(&in->pics, p, l);

    in->gctx = avfilter_graph_alloc();
    TAILQ_INIT(&in->filters);

#ifndef NDEBUG
    in->fp = fopen("abc.yuv", "wb+");
    in->afp = fopen("abc.pcm", "wb+");
#endif

    in->running = false;

    if (open_input(in) < 0) {
        goto clean;
    }

    struct AVCodecContext *ctx = NULL;
    char cmd[CMD_MAX_LEN];
    if (video) {
        if (!in->vctx) {
            ERROR("can't find video");
            goto clean;
        }
        ctx = in->vctx;

        /* buffer filter */
        snprintf(cmd, CMD_MAX_LEN,
                 "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
                 ctx->width, ctx->height, ctx->pix_fmt,
                 ctx->time_base.num,
                 ctx->time_base.den,
                 ctx->sample_aspect_ratio.num,
                 ctx->sample_aspect_ratio.den);
        in->src = create_filter("buffer", "in", cmd, in);
        if (in->src == NULL) {
            ERROR("%s create buffer filter failed", in->url);
            goto clean;
        }

        /* buffersink filter */
        in->sink = create_filter("buffersink", "out", "", in);
        if (in->sink == NULL) {
            ERROR("%s create buffer sink failed", in->url);
            goto clean;
        }
        enum AVPixelFormat pix_fmts[] = {AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE};
        rc = av_opt_set_int_list(in->sink->ctx, "pix_fmts", pix_fmts, AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
        if (rc < 0) {
            ERROR("%s set output pixel format failed: %s", in->url, av_err2str(rc));
        }


        snprintf(cmd, CMD_MAX_LEN, "/home/lyt/Pictures/a.png");
        struct filter *f1 = create_filter("movie", "m", cmd, in);
        if (f1 != NULL) {
            TAILQ_INSERT_TAIL(&in->filters, f1, l);
        }
        snprintf(cmd, CMD_MAX_LEN, "w=200:h=200");
        struct filter *f2 = create_filter("scale", "s", cmd, in);
        if (f2 != NULL) {
            TAILQ_INSERT_TAIL(&in->filters, f2, l);
        }
        snprintf(cmd, CMD_MAX_LEN, "10:10");
        struct filter *f3 = create_filter("overlay", "o", cmd, in);
        if (f3 != NULL) {
            TAILQ_INSERT_TAIL(&in->filters, f3, l);
        }




        snprintf(cmd, CMD_MAX_LEN, "/home/lyt/Pictures/b.png");
        struct filter *f4 = create_filter("movie", "m1", cmd, in);
        if (f4 != NULL) {
            TAILQ_INSERT_TAIL(&in->filters, f4, l);
        }
        snprintf(cmd, CMD_MAX_LEN, "w=200:h=200");
        struct filter *f5 = create_filter("scale", "s1", cmd, in);
        if (f5 != NULL) {
            TAILQ_INSERT_TAIL(&in->filters, f5, l);
        }
        snprintf(cmd, CMD_MAX_LEN, "W-w-64:H-h-32");
        struct filter *f6 = create_filter("overlay", "o1", cmd, in);
        if (f6 != NULL) {
            TAILQ_INSERT_TAIL(&in->filters, f6, l);
        }

        snprintf(cmd, CMD_MAX_LEN, "fontfile=DejaVuSansMono.ttf:text=ABCDEFG:fontsize=20:x=(w-text_w-60):y=(main_h-line_h-57):fontcolor=white@1.0:borderw=0.5:bordercolor=black@0.6");
        struct filter *f7 = create_filter("drawtext", "d1", cmd, in);
        if (f7 != NULL) {
            TAILQ_INSERT_TAIL(&in->filters, f7, l);
        }
        snprintf(cmd, CMD_MAX_LEN, "fontfile=DejaVuSansMono.ttf: text=TEMP30℃: fontsize=15: x=(w-text_w-100): y=(main_h-line_h-35): fontcolor=black@1.0: borderw=0.5: bordercolor=white@0.6");
        struct filter *f8 = create_filter("drawtext", "d2", cmd, in);
        if (f8 != NULL) {
            TAILQ_INSERT_TAIL(&in->filters, f8, l);
        }
        snprintf(cmd, CMD_MAX_LEN, "fontfile=DejaVuSansMono.ttf: text=this is a test:fontsize=20:x=if(gte(t,1), (main_w - mod(t*100,main_w +text_w)), NAN):y=400:fontcolor=white@1.0");
        struct filter *f9 = create_filter("drawtext", "d3", cmd, in);
        if (f9) {
            TAILQ_INSERT_TAIL(&in->filters, f9, l);
        }


        avfilter_link(in->src->ctx, 0, f3->ctx, 0);
        avfilter_link(f1->ctx, 0, f2->ctx, 0);
        avfilter_link(f2->ctx, 0, f7->ctx, 0);
        avfilter_link(f7->ctx, 0, f3->ctx, 1);

        avfilter_link(f3->ctx, 0, f6->ctx, 0);
        avfilter_link(f4->ctx, 0, f5->ctx, 0);
        avfilter_link(f5->ctx, 0, f8->ctx, 0);
        /* avfilter_link(f8->ctx, 0, f9->ctx, 0); */
        avfilter_link(f8->ctx, 0, f6->ctx, 1);
        avfilter_link(f6->ctx, 0, f9->ctx, 0);
        avfilter_link(f9->ctx, 0, in->sink->ctx, 0);

        /* avfilter_link(in->src->ctx, 0, in->sink->ctx, 0); */
    }

    /* AUDIO */
    if (audio) {
        if (!in->actx) {
            ERROR("can't find audio");
            goto clean;
        }
        ctx = in->actx;

        snprintf(cmd, CMD_MAX_LEN,
                 "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=0x%d",
                 ctx->time_base.num, ctx->time_base.den,
                 ctx->sample_rate, av_get_sample_fmt_name(ctx->sample_fmt),
                 (int)ctx->channel_layout);
        in->asrc = create_filter("abuffer", "ain", cmd, in);
        if (in->asrc == NULL) {
            ERROR("%s create abuffer filter failed", in->url);
            goto clean;
        }

        in->asink = create_filter("abuffersink", "aout", "", in);
        if (in->asink == NULL) {
            ERROR("%s create asinkbuffer filter failed", in->url);
            goto clean;
        }

        enum AVSampleFormat fmt = ASAMPLEFMT;
        rc = av_opt_set_bin(in->asink->ctx, "sample_fmts",
                            (uint8_t *)&fmt, sizeof(fmt),
                            AV_OPT_SEARCH_CHILDREN);
        if (rc < 0) {
            ERROR("%s set asinkbuffer sample_fmts failed: %s", in->url, av_err2str(rc));
            goto clean;
        }
        uint64_t layout = ACHANNEL;
        rc = av_opt_set_bin(in->asink->ctx, "channel_layouts",
                            (uint8_t *)&layout,
                            sizeof(layout),
                            AV_OPT_SEARCH_CHILDREN);
        if (rc < 0) {
            ERROR("%s set asinkbuffer channel_layouts failed: %s", in->url, av_err2str(rc));
            goto clean;
        }
        int rate = ASAMPLERATE;
        rc = av_opt_set_bin(in->asink->ctx, "sample_rates",
                            (uint8_t *)&rate,
                            sizeof(rate),
                            AV_OPT_SEARCH_CHILDREN);
        if (rc < 0) {
            ERROR("%s set asinkbuffer sample_rates failed: %s", in->url, av_err2str(rc));
            goto clean;
        }

        avfilter_link(in->asrc->ctx, 0, in->asink->ctx, 0);
        av_buffersink_set_frame_size(in->asink->ctx, 1024);
    }

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
    if (in->vctx) {
        avcodec_free_context(&in->vctx);
        in->vctx= NULL;
    }
    if (in->actx) {
        avcodec_free_context(&in->actx);
        in->actx = NULL;
    }

    if (in->fctx) {
        avformat_close_input(&in->fctx);
        in->fctx = NULL;
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
    if (in->asrc) {
        avfilter_free(in->asrc->ctx);
        free(in->asrc);
        in->asrc = NULL;
    }
    if (in->asink) {
        avfilter_free(in->asink->ctx);
        free(in->asink);
        in->asink = NULL;
    }
    if (in->gctx) {
        avfilter_graph_free(&in->gctx);
        in->gctx = NULL;
    }
    return NULL;
}
