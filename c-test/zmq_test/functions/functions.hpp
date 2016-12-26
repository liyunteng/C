/*
 * client_lib.hpp -- zmq client
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/07/14 19:23:34
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

#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP
#include "comm.h"

class Functions {
public:

    Functions();
    ~Functions();

    int init();
    int avStart();
    int avSetLayout(picture pic[4]);
    int avSetLayout4(int, int, int, int);
    int avSetLayout3(int, int, int);
    int avSetLayoutPip(int, int);
    int avSetEncSolution(const char *,const char *);
    int avSetEncBitsRate(const char *);
    int avSetEncIFramePeriod(const char *);
private:
    void *context;
    void *request;
    bool inited;
    int getReturn();
};

#endif
