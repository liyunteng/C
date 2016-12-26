/*
 * test_big.c -- test big or little
 *
 * Copyright (C) 2015 liyunteng
 * Auther: liyunteng <li_yunteng@163.com>
 * License: GPL
 * Update time:  2015/12/11 15:25:27
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
#include <stdio.h>
union T {
    char c[2];
    unsigned short i;
};

struct Y {
    unsigned char h:4, l:4;
};

int main(int argc, char *argv[])
{
    union T t;
    t.c[0] = 0x0;
    t.c[1] = 0x1;
    printf("t: %u\n", t.i);
    if (t.i == 256) {
	printf("little\n");
    } else {
	printf("big\n");
    }

    struct Y y;
    y.h = 0x0;
    y.l = 0x1;
    printf("y: %u, sizeof: %lu\n", y, sizeof(y));
    return 0;
}
