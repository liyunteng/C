/*  -*- compile-command: "clang -Wall -o ff main.c ff_decoder.c ff_encoder.c -g -lavcodec -lavformat -lswresample -lavutil -lavfilter -lpthread"; -*-
 * Filename: main.c
 * Description: main
 *
 * Copyright (C) 2017 StreamOcean
 *
 * Author: liyunteng <liyunteng@streamocean.com>
 * License: StreamOcean
 * Last-Updated: 2017/09/03 00:13:28
 */
#include <unistd.h>
#include "ff_decoder.h"
#include "ff_encoder.h"

int main(void)
{
    av_register_all();
    avformat_network_init();
    avfilter_register_all();
    struct stream_in *in = create_stream_in("http://172.16.1.238/live/hngqad1000");
    in->running = true;
    create_stream_out(in);
    /*
     * while (1) {
     *
     *     sleep(15);
     *     in->running = false;
     *     sleep(15);
     * }
     */

    pthread_join(in->pid, NULL);
    return 0;
}
