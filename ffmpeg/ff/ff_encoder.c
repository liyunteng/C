/*
 * Filename: ff_encoder.c
 * Description: ff encoder
 *
 * Copyright (C) 2017 StreamOcean
 *
 * Author: liyunteng <liyunteng@streamocean.com>
 * License: StreamOcean
 * Last-Updated: 2017/09/03 04:10:59
 */
#include "ff_encoder.h"
#define ENCODER_CODEC AV_CODEC_ID_H264
#define BITRATE       1000000
#define WIDTH         1280
#define HEIGHT        720
#define FRAMERATE     25
#define GOP           50

void *
out_run(void *arg)
{
    int rc;
    struct stream_out *out = (struct stream_out *)arg;
    if (out == NULL) {
        ERROR("invalid arg");
        return (void *)-1;
    }

    while (1) {
        if (out->current == NULL) {
            struct timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = 500 * 1000;
            select(0, NULL, NULL, NULL, &tv);
            continue;
        }

        rc = av_buffersink_get_frame(out->current->sink->ctx, out->frame);
        if (rc < 0) {
            if (rc == AVERROR(EAGAIN)) {
                struct timeval tv;
                tv.tv_sec = 0;
                tv.tv_usec = 10 * 1000;
                select(0, NULL, NULL, NULL, &tv);
                /* ERROR("get frame from buffersink failed: %s", av_err2str(rc)); */
            } else {
                ERROR("get frame from buffersink failed: %s", av_err2str(rc));
            }
            continue;
        }
        /*
         * if (out->fp) {
         *     int size = out->current->cctx->width * out->current->cctx->height;
         *     fwrite(out->frame->data[0], 1, size, out->fp);
         *     fwrite(out->frame->data[1], 1, size/4, out->fp);
         *     fwrite(out->frame->data[2], 1, size/4, out->fp);
         * }
         */
        out->frame->pict_type = AV_PICTURE_TYPE_NONE;
        out->frame->pts = out->pts++;
        rc = avcodec_send_frame(out->cctx, out->frame);
        if (rc < 0) {
            ERROR("send frame to encoder failed: %s", av_err2str(rc));
            continue;
        }

        while (avcodec_receive_packet(out->cctx, out->packet) >= 0) {
            out->packet->stream_index = out->fctx->streams[0]->index;
            av_packet_rescale_ts(out->packet,
                                 out->cctx->time_base,
                                 out->fctx->streams[out->packet->stream_index]->time_base);
            if (out->fp) {
                fwrite(out->packet->data, 1, out->packet->size, out->fp);
            }
            av_write_frame(out->fctx, out->packet);
            av_packet_unref(out->packet);
        }
        av_frame_unref(out->frame);
    }

    return (void *)0;
}

struct stream_out *
create_stream_out(struct stream_in *in)
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
    out->current = in;
    TAILQ_INIT(&out->ss);

    out->codec = avcodec_find_encoder(ENCODER_CODEC);
    if (out->codec == NULL) {
        ERROR("find codec %d failed", ENCODER_CODEC);
        goto clean;
    }

    out->cctx = avcodec_alloc_context3(out->codec);
    if (out->cctx == NULL) {
        ERROR("alloc context faild");
        goto clean;
    }

    out->cctx->codec_type = AVMEDIA_TYPE_VIDEO;
    out->cctx->qmin = 10;
    out->cctx->qmax = 31;

    out->cctx->bit_rate = BITRATE;
    out->cctx->width = WIDTH;
    out->cctx->height = HEIGHT;
    out->cctx->time_base = (AVRational){1, FRAMERATE};
    out->cctx->framerate = (AVRational){FRAMERATE, 1};

    out->cctx->gop_size = GOP;
    out->cctx->max_b_frames = 0;
    out->cctx->pix_fmt = AV_PIX_FMT_YUV420P;

    if (out->codec->id == AV_CODEC_ID_H264) {
        av_opt_set(out->cctx->priv_data, "preset", "slow", 0);
        av_opt_set(out->cctx->priv_data, "tune", "zerolatency", 0);
    }

    rc = avcodec_open2(out->cctx, out->codec, NULL);
    if (rc < 0) {
        ERROR("open codec failed");
        goto clean;
    }

    avformat_alloc_output_context2(&out->fctx, NULL, "mpegts", out_path);
    if (out->fctx == NULL) {
        ERROR("alloc format ctx failed");
        goto clean;
    }

    AVStream *video_stream;
    video_stream = avformat_new_stream(out->fctx, NULL);
    if (video_stream == NULL) {
        ERROR("create stream failed");
        goto clean;
    }

    rc = avcodec_parameters_from_context(video_stream->codecpar, out->cctx);
    if (rc != 0) {
        ERROR("copy codecpar failed");
        goto clean;
    }

    if (out->fctx->oformat->flags & AVFMT_GLOBALHEADER) {
        out->cctx->flags |= CODEC_FLAG_GLOBAL_HEADER;
    }

    video_stream->time_base = out->cctx->time_base;

    INFO("OUTPUT:\n");
    av_dump_format(out->fctx, 0, out_path, 1);
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
    /* out->fp = fopen("abc.yuv", "wb+"); */



    out->frame = av_frame_alloc();
    if (out->frame == NULL) {
        ERROR("alloc frame failed");
        goto clean;
    }
    out->frame->format = out->cctx->pix_fmt;
    out->frame->width = out->cctx->width;
    out->frame->height = out->cctx->height;

    rc = av_frame_get_buffer(out->frame, 32);
    if (rc < 0) {
        ERROR("alloca video frame data failed");
        goto clean;
    }

    out->packet = av_packet_alloc();
    if (out->packet == NULL) {
        ERROR("alloc packet failed");
        goto clean;
    }


    rc = pthread_create(&out->pid, NULL, out_run, out);
    if (rc != 0) {
        ERROR("create out thread failed");
        goto clean;
    }
    return out;

  clean:
    if (out->codec) {
        out->codec = NULL;
    }
    if (out->cctx) {
        avcodec_free_context(&out->cctx);
        out->cctx = NULL;
    }
    if (out->frame) {
        av_frame_free(&out->frame);
        out->frame = NULL;
    }
    if (out->packet) {
        av_packet_free(&out->packet);
        out->packet = NULL;
    }
    return NULL;
}
