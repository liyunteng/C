/*
 * Description: main
 *
 * Copyright (C) 2017 StreamOcean
 * Last-Updated: <2017/09/19 10:21:24 liyunteng>
 */

#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <time.h>
#include <pthread.h>
#include "ff_decoder.h"
#include "ff_encoder.h"

int main(void)
{
    av_register_all();
    avformat_network_init();
    avfilter_register_all();

    struct stream_out *out = create_stream_out("udp://127.0.0.1:12345");

    /*
     * struct stream_in *music = create_stream_in("./qrsy.mp2", out, false, true);
     * add_to_out(music, out);
     */

    struct stream_in *in1 = NULL, *in2 = NULL;
    /* start(music); */
    while (1) {
        in1 = create_stream_in("http://172.16.1.238/live/hngqad1000", out,  true, true);
        in1->pre = in2;
        sleep(5);

        in2 = create_stream_in("http://172.16.1.238/live/hngq1000", out, true, true);
        in2->pre = in1;
        sleep(5);
    }


    pthread_join(in1->pid, NULL);
    /* pthread_join(in2->pid, NULL); */
    /* pthread_join(out->pid, NULL); */

    return 0;
}
