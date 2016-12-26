/*
 * hwclient.cpp -- hello world
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/08/25 18:04:49
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

#include <zmq.hpp>
#include <iostream>
#include <string>
#include <assert.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
        zmq::context_t context(1);
        zmq::socket_t socket(context, ZMQ_REQ);
        socket.connect("tcp://localhost:5555");

        int i;
        for (i = 0; i != 10; i++) {
                int ret = socket.send("hello", sizeof("hello"), 0);
                assert(ret == sizeof("hello"));

                char buf[10];
                socket.recv(buf, sizeof(buf), 0);
                std::cout << "Receiv " << std::string(buf) << std::endl;
        }
        return 0;
}
