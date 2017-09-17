/*
 * Description: ff encoder
 *
 * Copyright (C) 2017 StreamOcean
 * Last-Updated: <2017/09/17 09:51:20 liyunteng>
 */

#include "ff_encoder.h"
#include <libavutil/timestamp.h>
#include <sys/time.h>


static void
print_packet(const AVFormatContext *fmt_ctx, const AVPacket *pkt, const char *tag)
{
    AVRational *time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;
    struct timeval tv;
    gettimeofday(&tv, NULL);

    DEBUG("%s: time %lums pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d",
          tag,
          tv.tv_sec*1000 + tv.tv_usec/1000,
          av_ts2str(pkt->pts), av_ts2timestr(pkt->pts, time_base),
          av_ts2str(pkt->dts), av_ts2timestr(pkt->dts, time_base),
          av_ts2str(pkt->duration), av_ts2timestr(pkt->duration, time_base),
          pkt->stream_index);
}


static struct filter *
create_filter(const char *filter_name, const char *name, const char *cmd, struct stream_out *out)
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
    int rc = avfilter_graph_create_filter(&f->ctx,f->f,name, arg, NULL, out->gctx);
    if (rc < 0) {
        ERROR("add filter [%s:%s] failed: %s", f->fname, f->name, av_err2str(rc));
        free(f);
        return NULL;
    }
    INFO("add filter [%s:%s] cmd: %s", f->fname, f->name, f->cmd);
    return f;
}


void *
out_a_run(void *arg)
{
    int rc;
    struct stream_out *out = (struct stream_out *)arg;
    if (out == NULL) {
        ERROR("invalid arg");
        return (void *)-1;
    }

    AVPacket *packet = av_packet_alloc();
    if (packet == NULL) {
        ERROR("alloc packet failed");
        return (void *)-1;
    }

    while (1) {
        if (out->current_audio == NULL) {
            struct timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = 10 * 1000;
            select(0, NULL, NULL, NULL, &tv);
            continue;
        }
        AVFrame *aframe = av_frame_alloc();
        if (aframe == NULL) {
            ERROR("alloc frame failed");
            return (void *)-1;
        }

        if (out->current_audio->running && (av_buffersink_get_frame(out->current_audio->asink->ctx, aframe) >= 0)) {
            aframe->pict_type = AV_PICTURE_TYPE_NONE;
            /* DEBUG("audio frame pts: %lu", frame->pts); */
            /* frame->pts = AV_NOPTS_VALUE; */
            out->apts += aframe->nb_samples;
            aframe->pts = out->apts;
            rc = avcodec_send_frame(out->cctx[1], aframe);
            if (rc < 0) {
                ERROR("send frame to encoder failed: %s", av_err2str(rc));
            } else {
                while (avcodec_receive_packet(out->cctx[1], packet) >= 0) {
                    packet->stream_index = out->fctx->streams[1]->index;
                    av_packet_rescale_ts(packet,
                                         out->cctx[1]->time_base,
                                         out->fctx->streams[1]->time_base);

                    print_packet(out->fctx, packet, "audio");
                    if (out->afp) {
                        fwrite(packet->data, 1, packet->size, out->afp);
                    }
                    /* DEBUG("packet audio pts: %lu, out->pts: %lu", packet->pts, out->apts); */
                    av_interleaved_write_frame(out->fctx, packet);
                    av_packet_unref(packet);
                }
            }
        }
        av_frame_free(&aframe);
    }
    return (void *)0;
}

void *
out_run(void *arg)
{
    int rc;
    struct stream_out *out = (struct stream_out *)arg;
    if (out == NULL) {
        ERROR("invalid arg");
        return (void *)-1;
    }

    /*
     * frame->format = out->cctx[0]->pix_fmt;
     * frame->width = out->cctx[0]->width;
     * frame->height = out->cctx[0]->height;
     * rc = av_frame_get_buffer(frame, 32);
     * if (rc < 0) {
     *     ERROR("alloca video frame data failed");
     *     return (void *)-1;
     * }
     */

    AVPacket *packet = av_packet_alloc();
    if (packet == NULL) {
        ERROR("alloc packet failed");
        return (void *)-1;
    }

    unsigned last_video_pts = 0;
    while (1) {
        if (out->current_video == NULL &&
            out->current_audio == NULL) {
            struct timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = 10 * 1000;
            select(0, NULL, NULL, NULL, &tv);
            continue;
        }
        AVFrame *frame = av_frame_alloc();
        if (frame == NULL) {
            ERROR("alloc frame failed");
            return (void *)-1;
        }
        if (out->current_video->running && (av_buffersink_get_frame(out->current_video->sink->ctx, frame) >= 0)) {
            frame->pict_type = AV_PICTURE_TYPE_NONE;
            /* DEBUG("video frame pts: %lu", frame->pts); */
            frame->pts = out->pts++;
            rc = avcodec_send_frame(out->cctx[0], frame);
            if (rc < 0) {
                ERROR("send frame to encoder failed: %s", av_err2str(rc));
            } else {
                while (avcodec_receive_packet(out->cctx[0], packet) >= 0) {

                    packet->stream_index = out->fctx->streams[0]->index;
                    av_packet_rescale_ts(packet,
                                         out->cctx[0]->time_base,
                                         out->fctx->streams[0]->time_base);
                    packet->duration = packet->pts - last_video_pts;
                    last_video_pts = packet->pts;

                    print_packet(out->fctx, packet, "video");
                    if (out->fp) {
                        fwrite(packet->data, 1, packet->size, out->fp);
                    }
                    av_interleaved_write_frame(out->fctx, packet);
                    av_packet_unref(packet);
                }
            }
            /* av_frame_unref(frame); */
        }
        av_frame_free(&frame);

        frame = av_frame_alloc();
        if (frame == NULL) {
            ERROR("alloc frame failed");
            return (void *)-1;
        }
        if (out->current_audio->running && (av_buffersink_get_frame(out->current_audio->asink->ctx, frame) >= 0)) {
            frame->pict_type = AV_PICTURE_TYPE_NONE;
            out->apts += frame->nb_samples;
            frame->pts = out->apts;
            /*
             * DEBUG("sample rate: %d, nb sample: %d",
             *       frame->sample_rate,
             *       frame->nb_samples);
             */
            rc = avcodec_send_frame(out->cctx[1], frame);
            if (rc < 0) {
                ERROR("send frame to encoder failed: %s", av_err2str(rc));
            } else {
                while (avcodec_receive_packet(out->cctx[1], packet) >= 0) {
                    packet->stream_index = out->fctx->streams[1]->index;
                    av_packet_rescale_ts(packet,
                                         out->cctx[1]->time_base,
                                         out->fctx->streams[1]->time_base);

                    print_packet(out->fctx, packet, "audio");
                    if (out->afp) {
                        fwrite(packet->data, 1, packet->size, out->afp);
                    }
                    av_interleaved_write_frame(out->fctx, packet);
                    av_packet_unref(packet);
                }
            }
            /* av_frame_unref(frame); */
        }
        av_frame_free(&frame);

    }
    return (void *)0;
}

struct stream_out *
create_stream_out(struct stream_head streams)
{
    const char *out_path = "udp://127.0.0.1:12345";
    int rc;
    struct stream_out *out = (struct stream_out *)calloc(1, sizeof(struct stream_out));
    if (out == NULL) {
        ERROR("calloc failed");
        return NULL;
    }
    /* out->current = NULL; */
    out->pts = 0;
    out->apts = 0;
    out->current_video = NULL;
    out->current_audio = NULL;
    TAILQ_INIT(&out->ss);

    avformat_alloc_output_context2(&out->fctx, NULL, "mpegts", out_path);
    if (out->fctx == NULL) {
        ERROR("alloc format ctx failed");
        goto clean;
    }


    out->cctx = av_mallocz_array(2, sizeof(AVCodecContext *));
    if (out->cctx == NULL) {
        ERROR("malloc codec context failed: %s", av_err2str(AVERROR(ENOMEM)));
        goto clean;
    }

    AVCodec *codec = avcodec_find_encoder(VENCODER_CODEC);
    if (codec == NULL) {
        ERROR("find codec %d failed", VENCODER_CODEC);
        goto clean;
    }

    out->cctx[0] = avcodec_alloc_context3(codec);
    if (out->cctx == NULL) {
        ERROR("alloc context faild");
        goto clean;
    }

    out->cctx[0]->codec_type = AVMEDIA_TYPE_VIDEO;
    out->cctx[0]->qmin = 10;
    out->cctx[0]->qmax = 31;

    out->cctx[0]->bit_rate = VBITRATE;
    out->cctx[0]->width = VWIDTH;
    out->cctx[0]->height = VHEIGHT;
    out->cctx[0]->time_base = (AVRational){1, VFRAMERATE};
    out->cctx[0]->framerate = (AVRational){VFRAMERATE, 1};

    out->cctx[0]->gop_size = VGOP;
    out->cctx[0]->max_b_frames = 0;
    out->cctx[0]->pix_fmt = VPIXFMT;
    out->cctx[0]->codec_tag = 0;

    out->cctx[0]->thread_count = 2;
    out->cctx[0]->flags |= AV_CODEC_FLAG_LOW_DELAY;
    out->cctx[0]->flags |= AV_CODEC_FLAG2_FAST;
    out->cctx[0]->flags |= AV_CODEC_FLAG2_LOCAL_HEADER;

    if (codec->id == AV_CODEC_ID_H264) {
        av_opt_set(out->cctx[0]->priv_data, "preset", "fast", 0);
        av_opt_set(out->cctx[0]->priv_data, "tune", "zerolatency", 0);
    }

    rc = avcodec_open2(out->cctx[0], codec, NULL);
    if (rc < 0) {
        ERROR("open codec failed");
        goto clean;
    }

    AVStream *video_stream;
    video_stream = avformat_new_stream(out->fctx, NULL);
    if (video_stream == NULL) {
        ERROR("create stream failed");
        goto clean;
    }

    rc = avcodec_parameters_from_context(video_stream->codecpar, out->cctx[0]);
    if (rc != 0) {
        ERROR("copy codecpar failed");
        goto clean;
    }
    video_stream->time_base = out->cctx[0]->time_base;

    /* AUDIO */
    codec = NULL;
    codec = avcodec_find_encoder(AENCODER_CODEC);
    if (codec == NULL) {
        ERROR("find audio encoder failed");
        goto clean;
    }
    out->cctx[1] = avcodec_alloc_context3(codec);
    if (out->cctx[1] == NULL) {
        ERROR("alloc audio codec context failed");
        goto clean;
    }
    out->cctx[1]->codec_type = AVMEDIA_TYPE_AUDIO;
    out->cctx[1]->sample_rate = ASAMPLERATE;
    out->cctx[1]->channel_layout = ACHANNEL;
    out->cctx[1]->channels = av_get_channel_layout_nb_channels(out->cctx[1]->channel_layout);
    out->cctx[1]->sample_fmt = ASAMPLEFMT;
    out->cctx[1]->time_base = (AVRational){1, ASAMPLERATE};
    out->cctx[1]->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
    out->cctx[1]->codec_tag = 0;

    rc = avcodec_open2(out->cctx[1], codec, NULL);
    if (rc < 0) {
        ERROR("open codec failed");
        goto clean;
    }

    AVStream *audio_stream;
    audio_stream = avformat_new_stream(out->fctx, NULL);
    if (audio_stream == NULL) {
        ERROR("create audio stream failed");
        goto clean;
    }

    rc = avcodec_parameters_from_context(audio_stream->codecpar, out->cctx[1]);
    if (rc != 0) {
        ERROR("copy codecpar failed");
        goto clean;
    }

    INFO("OUTPUT:\n");
    av_dump_format(out->fctx, 0, out_path, 1);

    if (out->fctx->oformat->flags & AVFMT_GLOBALHEADER) {
        out->cctx[0]->flags |= CODEC_FLAG_GLOBAL_HEADER;
    }
    if (! (out->fctx->oformat->flags & AVFMT_NOFILE)) {
        if (avio_open(&out->fctx->pb, out_path, AVIO_FLAG_WRITE) < 0) {
            ERROR("avio open failed");
            goto clean;
        }
    }
    if (avformat_write_header(out->fctx, NULL) < 0) {
        ERROR("can not write header");
        goto clean;
    }

#ifndef NDEBUG
    out->fp = fopen("abc.h264", "wb+");
    out->afp = fopen("abc.aac", "wb+");
#endif

    out->gctx = avfilter_graph_alloc();
    struct AVCodecContext *ctx = out->cctx[0];
    char cmd[CMD_MAX_LEN];
    /****************
     * VIDEO FILTER *
     ****************/
    snprintf(cmd, CMD_MAX_LEN,
             "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
             ctx->width, ctx->height, ctx->pix_fmt,
             ctx->time_base.num,
             ctx->time_base.den,
             ctx->sample_aspect_ratio.num,
             ctx->sample_aspect_ratio.den);
    out->src = create_filter("buffer", "in", cmd, out);
    if (!out->src) {
        ERROR("create buffer filter failed");
        goto clean;
    }
    out->sink = create_filter("buffersink", "out", "", out);
    if (!out->src) {
        ERROR("create buffer filter failed");
        goto clean;
    }
    enum AVPixelFormat pix_fmts[] = {AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE};
    rc = av_opt_set_int_list(out->sink->ctx, "pix_fmts", pix_fmts, AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
    if (rc < 0) {
        ERROR("set output pixel format failed: %s", av_err2str(rc));
    }

    avfilter_link(out->src->ctx, 0, out->sink->ctx, 0);

    /****************
     * AUDIO FILTER *
     ****************/
    ctx = out->cctx[1];
    snprintf(cmd, CMD_MAX_LEN,
             "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=0x%d",
             ctx->time_base.num, ctx->time_base.den,
             ctx->sample_rate, av_get_sample_fmt_name(ctx->sample_fmt),
             (int)ctx->channel_layout);
    out->asrc = create_filter("abuffer", "ain", cmd, out);
    if (out->asrc == NULL) {
        ERROR("create abuffer filter failed");
        goto clean;
    }

    out->asink = create_filter("abuffersink", "aout", "", out);
    if (out->asink == NULL) {
        ERROR("create asinkbuffer filter failed");
        goto clean;
    }
    rc = av_opt_set_bin(out->asink->ctx, "sample_fmts",
                        (uint8_t *)&ctx->sample_fmt, sizeof(ctx->sample_fmt),
                        AV_OPT_SEARCH_CHILDREN);
    if (rc < 0) {
        ERROR("set asinkbuffer sample_fmts failed: %s", av_err2str(rc));
        goto clean;
    }
    rc = av_opt_set_bin(out->asink->ctx, "channel_layouts",
                        (uint8_t *)&ctx->channel_layout,
                        sizeof(ctx->channel_layout),
                        AV_OPT_SEARCH_CHILDREN);
    if (rc < 0) {
        ERROR("set asinkbuffer channel_layouts failed: %s", av_err2str(rc));
        goto clean;
    }
    rc = av_opt_set_bin(out->asink->ctx, "sample_rates",
                        (uint8_t *)&ctx->sample_rate,
                        sizeof(ctx->sample_rate),
                        AV_OPT_SEARCH_CHILDREN);
    if (rc < 0) {
        ERROR("set asinkbuffer sample_rates failed: %s", av_err2str(rc));
        goto clean;
    }
    avfilter_link(out->asrc->ctx, 0, out->asink->ctx, 0);

    rc = avfilter_graph_config(out->gctx, NULL);
    if (rc < 0) {
        ERROR("av filter graph config failed: %s", av_err2str(rc));
        goto clean;
    }


    rc = pthread_create(&out->pid, NULL, out_run, out);
    if (rc != 0) {
        ERROR("create out thread failed");
        goto clean;
    }

    /*
     * rc = pthread_create(&out->a_pid, NULL, out_a_run, out);
     * if (rc != 0) {
     *     ERROR("create out thread failed");
     *     goto clean;
     * }
     */

    return out;

  clean:
    if (out->cctx) {
        avcodec_free_context(&out->cctx[0]);
        avcodec_free_context(&out->cctx[1]);
        out->cctx[0] = NULL;
        out->cctx[1] = NULL;
        av_free(out->cctx);
        out->cctx = NULL;
    }

    if (out->gctx) {
        avfilter_graph_free(&out->gctx);
        out->gctx = NULL;
    }
    if (out->fctx) {
        avformat_free_context(out->fctx);
        out->fctx = NULL;
    }
    if (out->fp) {
        fclose(out->fp);
    }
    free(out);
    out = NULL;

    return NULL;
}
