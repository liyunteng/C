#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <time.h>
#include <getopt.h>

#include "pic_ctl.h"

struct option longopts[] = {
	{"index", 1, NULL, 'i'},
	{"delay", 1, NULL, 'd'},
	{0, 0, 0, 0}
};

int idx = 0;
int delay = 0;

void usage()
{
	fprintf(stderr, "disk_reset --index|-i <idx> [--delay|-d <seconds>]\n"
		"disk_reset --help\n");
	exit(-1);
}

int parser(int argc, char *argv[])
{
	int c, freq = 0;
	while ((c=getopt_long(argc, argv, "i:d:h", longopts, NULL)) != -1) {
		switch (c) {
		case 'i':
			if (optarg)
				idx = atoi(optarg);
		case 'd':
			if (optarg)
				delay = atoi(optarg);
			break;
		case 'h':
		case -1:
		case '?':
			return -1;
		}
	}

	return 0;
}

int main(int argc, char *argv[])
{
	int i;

	if (parser(argc, argv) < 0) {
		usage();
		return -1;
	}

	if (idx > 16 || idx < 1) {
		fprintf(stderr, "input idx error.\n");
		return -1;
	}

	if (delay > 0)
		pic_reset_timer(delay);

	pic_reset_hd(idx - 1);

	return 0;
}

