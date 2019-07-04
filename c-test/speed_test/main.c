/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  04/09/2016 10:52:42 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  liyunteng (), li_yunteng@163.com
 *   Organization:
 *
 * =====================================================================================
 */
#include <stdio.h>
#include "speed.h"

#define PORT 6666

int main(int argc, char *argv[])
{
    int port = PORT;
    int interval = 10;
    if (argc < 2) {
	fprintf(stderr, "Usage: %s host [port [interval]].\n", argv[0]);
	return -1;
    }



    if (argc == 3)
	port = atoi(argv[2]);

    if (argc >= 4) {
	port = atoi(argv[2]);
	interval = atoi(argv[3]);
    }

    return tcp_speed_test(argv[1], port, interval);

}

/* Local Variables: */
/* compile-command: "clang -Wall -o main main.c -g speed.c" */
/* End: */
