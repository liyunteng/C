#include "functions.hpp"
#include <unistd.h>

int main(int argc, char **argv)
{
	picture layout[4] = {
		{320, 320, 960, 540, 1, 1},
		{0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0},
	};

	Functions *f = new Functions();
	f->init();
	f->avStart();
	sleep(3);
	while (1) {
	f->avSetLayout4(1, 2, 3, 0xffff);
	sleep(10);
	f->avSetLayout3(1, 2, 0xffff);
	sleep(10);
	f->avSetLayoutPip(1, 2);
	sleep(10);
	f->avSetLayout(layout);
	sleep(10);
	f->avSetEncSolution("1280", "720");
	sleep(3);
	f->avSetEncBitsRate("1920000");
	sleep(3);
	f->avSetEncIFramePeriod("60");
	sleep(3);
	}
	f->avSetLayout4(1, 2, 3, 0xffff);
	return 0;
}

