/*  -*- compile-command: "clang -Wall -o ff main.c ff_decoder.c ff_encoder.c -g -lavcodec -lavformat -lswresample -lavutil -lavfilter -lpthread"; -*-
 * Filename: main.c
 * Description: main
 *
 * Copyright (C) 2017 StreamOcean
 *
 * Author: liyunteng <liyunteng@streamocean.com>
 * License: StreamOcean
 * Last-Updated: 2017/09/03 20:52:38
 */
#include <unistd.h>
#include "ff_decoder.h"
#include "ff_encoder.h"

int main(void)
{
    av_register_all();
    avformat_network_init();
    avfilter_register_all();
    struct stream_in *in1 = create_stream_in("http://172.16.1.238/live/hngqad1000");
    /* struct stream_in *in2 = create_stream_in("http://172.16.1.238/live/hngq1000"); */
    struct stream_head ss;
    TAILQ_INIT(&ss);
    TAILQ_INSERT_TAIL(&ss, in1, l);
    /* TAILQ_INSERT_TAIL(&ss, in2, l); */
    struct stream_out *out = create_stream_out(ss);
    start(in1);
    out->current = in1;
    /*
     * while (1) {
     *     stop(in2);
     *     start(in1);
     *     out->current = in1;
     *     /\*
     *      * avcodec_send_frame(out->cctx[0], NULL);
     *      * avcodec_send_frame(out->cctx[1], NULL);
     *      * avcodec_flush_buffers(out->cctx[0]);
     *      * avcodec_flush_buffers(out->cctx[1]);
     *      *\/
     *     if (avformat_write_header(out->fctx, NULL) < 0) {
     *         ERROR("can not write header");
     *     }
     *     INFO("frames: %lu", out->fctx->streams[0]->nb_frames);
     *     sleep(15);
     *
     *
     *     stop(in1);
     *     start(in2);
     *     out->current = in2;
     *     /\*
     *      * avcodec_flush_buffers(out->cctx[0]);
     *      * avcodec_flush_buffers(out->cctx[1]);
     *      *\/
     *     if (avformat_write_header(out->fctx, NULL) < 0) {
     *         ERROR("can not write header");
     *     }
     *     INFO("frames: %lu", out->fctx->streams[0]->nb_frames);
     *     sleep(15);
     *
     * }
     */


    pthread_join(in1->pid, NULL);
    /* pthread_join(in2->pid, NULL); */
    return 0;
}
