/*
 * Filename: ff_encoder.c
 * Description: ff encoder
 *
 * Copyright (C) 2017 StreamOcean
 *
 * Author: liyunteng <liyunteng@streamocean.com>
 * License: StreamOcean
 * Last-Updated: 2017/09/03 22:49:02
 */
#include "ff_encoder.h"
#define VENCODER_CODEC AV_CODEC_ID_H264
#define AENCODER_CODEC AV_CODEC_ID_AAC

#define VBITRATE    1000000
#define VWIDTH      1280
#define VHEIGHT     720
#define VFRAMERATE  25
#define VGOP        50
#define VPIXFMT     AV_PIX_FMT_YUV420P

#define ASAMPLERATE 44100
#define ASAMPLEFMT  AV_SAMPLE_FMT_FLTP
#define ACHANNEL    AV_CH_LAYOUT_STEREO

void *
out_run(void *arg)

{
    int rc;
    struct stream_out *out = (struct stream_out *)arg;
    if (out == NULL) {
        ERROR("invalid arg");
        return (void *)-1;
    }

    AVFrame *frame = av_frame_alloc();
    if (frame == NULL) {
        ERROR("alloc frame failed");
        return (void *)-1;
    }

    frame->format = out->cctx[0]->pix_fmt;
    frame->width = out->cctx[0]->width;
    frame->height = out->cctx[0]->height;
    frame->nb_samples = out->cctx[1]->frame_size;
    frame->format = out->cctx[1]->sample_fmt;
    frame->channel_layout = out->cctx[1]->channel_layout;
    rc = av_frame_get_buffer(frame, 0);
    if (rc < 0) {
        ERROR("alloca video frame data failed");
        return (void *)-1;
    }

    AVPacket *packet = av_packet_alloc();
    if (packet == NULL) {
        ERROR("alloc packet failed");
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

        if (av_buffersink_get_frame(out->current->sink->ctx, frame) >= 0) {
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
                    /*
                     * DEBUG("video codec time_base: %d/%d,  fmt time_base: %d/%d",
                     *       out->cctx[0]->time_base.num,
                     *       out->cctx[0]->time_base.den,
                     *       out->fctx->streams[0]->time_base.num,
                     *       out->fctx->streams[0]->time_base.den
                     *     );
                     */
                    if (out->fp) {
                        fwrite(packet->data, 1, packet->size, out->fp);
                    }
                    /* DEBUG("packet video pts: %lu, out pts: %lu", packet->pts, out->pts); */
                    av_interleaved_write_frame(out->fctx, packet);
                    av_packet_unref(packet);
                }
            }
            av_frame_unref(frame);
        }

        if (av_buffersink_get_frame(out->current->asink->ctx, frame) >= 0) {
            frame->pict_type = AV_PICTURE_TYPE_NONE;
            /* DEBUG("audio frame pts: %lu", frame->pts); */
            /* frame->pts = AV_NOPTS_VALUE; */
            frame->pts = out->apts;
            out->apts += frame->nb_samples;
            rc = avcodec_send_frame(out->cctx[1], frame);
            if (rc < 0) {
                ERROR("send frame to encoder failed: %s", av_err2str(rc));
            } else {
                while (avcodec_receive_packet(out->cctx[1], packet) >= 0) {
                    packet->stream_index = out->fctx->streams[1]->index;
                    av_packet_rescale_ts(packet,
                                         out->cctx[1]->time_base,
                                         out->fctx->streams[1]->time_base);
                    /*
                     * DEBUG("audio codec time_base: %d/%d,  fmt time_base: %d/%d",
                     *       out->cctx[1]->time_base.num,
                     *       out->cctx[1]->time_base.den,
                     *       out->fctx->streams[1]->time_base.num,
                     *       out->fctx->streams[1]->time_base.den
                     *     );
                     */
                    if (out->fp) {
                        fwrite(packet->data, 1, packet->size, out->fp);
                    }
                    /* DEBUG("packet audio pts: %lu, out->pts: %lu", packet->pts, out->apts); */
                    av_interleaved_write_frame(out->fctx, packet);
                    av_packet_unref(packet);
                }
            }
            av_frame_unref(frame);
        }

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
    out->current = NULL;
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

    if (codec->id == AV_CODEC_ID_H264) {
        av_opt_set(out->cctx[0]->priv_data, "preset", "slow", 0);
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

    if (out->fctx->oformat->flags & AVFMT_GLOBALHEADER) {
        out->cctx[0]->flags |= CODEC_FLAG_GLOBAL_HEADER;
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

#ifdef FORDEBUG
    out->fp = fopen("abc.ts", "wb+");
#endif

    rc = pthread_create(&out->pid, NULL, out_run, out);
    if (rc != 0) {
        ERROR("create out thread failed");
        goto clean;
    }
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
