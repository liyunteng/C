/*
 * client.h -- client header
 *
 * Copyright (C) 2016 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2016/04/08 16:08:47
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

#define BUFFSIZE 4096

#define MSG_SEND        0
#define MSG_RECV        1

struct myhdr {
        uint8_t type;
        uint32_t seq;
        uint16_t id;
        uint8_t data[0];
#define HSIZE 5
};


#endif
