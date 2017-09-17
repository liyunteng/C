/* -*- compile-command: "clang -Wall -o ff main.c ff_decoder.c ff_encoder.c -g -lavcodec -lavformat -lswresample -lavutil -lavfilter -lpthread -DNDEBUG"; -*- */
/*
 * Description: main
 *
 * Copyright (C) 2017 StreamOcean
 * Last-Updated: <2017/09/17 09:12:07 liyunteng>
 */

#include <unistd.h>
#include "ff_decoder.h"
#include "ff_encoder.h"

int main(void)
{
    av_register_all();
    avformat_network_init();
    avfilter_register_all();
    struct stream_in *in1 = create_stream_in("http://172.16.1.238/live/hngqad1000", true, true);
    struct stream_in *in2 = create_stream_in("http://172.16.1.238/live/hngq1000", true, true);
    /* struct stream_in *music = create_stream_in("./qrsy.mp2", false, true); */
    struct stream_head ss;
    TAILQ_INIT(&ss);
    TAILQ_INSERT_TAIL(&ss, in1, l);
    TAILQ_INSERT_TAIL(&ss, in2, l);
    /* TAILQ_INSERT_TAIL(&ss, music, l); */

    /* start(music); */
    struct stream_out *out = create_stream_out(ss);
    /* out->current_audio = music; */
    while (1) {
        start(in1);
        out->current_video = in1;
        out->current_audio = in1;
        stop(in2);
        sleep(20);

        start(in2);
        out->current_video = in2;
        out->current_audio = in2;
        stop(in1);
        sleep(20);
    }


    pthread_join(in1->pid, NULL);
    pthread_join(in2->pid, NULL);
    return 0;
}
