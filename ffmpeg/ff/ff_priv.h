/*
 * Description: priv
 *
 * Copyright (C) 2017 StreamOcean
 * Last-Updated: <2017/09/17 06:52:40 liyunteng>
 */
#ifndef FF_PRIV_H
#define FF_PRIV_H

#define URL_MAX_LEN 256
#define CMD_MAX_LEN 1024

#define LOGPREFIX "[%s:%d] "
#define LOGPOSTFIX "\n"
#define LOGVAR __FUNCTION__, __LINE__
#define DEBUG(format, ...)   printf(LOGPREFIX format LOGPOSTFIX, LOGVAR, ##__VA_ARGS__)
#define INFO(format, ...)    printf(LOGPREFIX format LOGPOSTFIX, LOGVAR, ##__VA_ARGS__)
#define WARNING(format, ...) printf(LOGPREFIX format LOGPOSTFIX, LOGVAR, ##__VA_ARGS__)
#define ERROR(format, ...)   printf(LOGPREFIX format LOGPOSTFIX, LOGVAR, ##__VA_ARGS__)


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

#endif
