/*
 * comm.h -- comm.h
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/07/14 18:57:13
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#ifndef COMM_H
#define COMM_H
#include <stdint.h>

#define ERR_SUCCESS 0
#define ERR_NOT_INIT    -1
#define ERR_ARG_INVALID -2


#define CMD_AVSETLAYOUT                 0x01
#define CMD_AVSTART                     0x02
#define CMD_AVSETENC_SOLUTION           0x03
#define CMD_AVSETENC_BITSRATE           0x04
#define CMD_AVSETENC_IFRAMEPERIOD       0x05
#define CMD_AVINIT                      0x06
#define CMD_TEST                        0x10

#define VIDEO_ENC_PARAM_LEN               12

typedef struct _picture{
	uint32_t 	startX;
	uint32_t	startY;
	uint32_t	width;
	uint32_t	height;
	int32_t		streamId;
	int32_t		volume;
   
}picture;

typedef struct _video_enc_param{
	int8_t encWidth[VIDEO_ENC_PARAM_LEN];	//[352 .. 1920]
	int8_t encHeight[VIDEO_ENC_PARAM_LEN];	//[288 .. 1080]
	int8_t encBitRate[VIDEO_ENC_PARAM_LEN];	//[384000 512000 768000 1024000(1M) 1920000(2M) 3840000(4M) 7680000(8M)]
	int8_t IFramePeriod[VIDEO_ENC_PARAM_LEN];//[30(1s) 60(2s) 120(4s) ...]

}video_enc_param;


typedef struct _header{
    int32_t cmd;
}Header;

typedef struct _paramSetAVLayout{
    Header h;
    picture pictures[4];
}ParamSetAVLayout;

typedef struct _paramAVStart {
    Header h;
}ParamAVStart;

typedef struct _paramAVSetEncSolution {
    Header h;
    int8_t width[VIDEO_ENC_PARAM_LEN];
    int8_t height[VIDEO_ENC_PARAM_LEN];
}ParamAVSetEncSolution;

typedef struct _paramAVSetEncBitsRate {
    Header h;
    int8_t bitsRate[VIDEO_ENC_PARAM_LEN];
}ParamAVSetEncBitsRate;

typedef struct _paramAVSetEncIFramePeriod {
    Header h;
    int8_t period[VIDEO_ENC_PARAM_LEN];
}ParamAVSetEncIFramePeriod;

typedef struct  _paramAVInit {
    Header h;
    video_enc_param param;
}ParamAVInit;

typedef struct _paramTest {
    Header h;
    char test[128];
}ParamTest;
#endif
