#include "watchdog_lib.h"
#include <unistd.h>

#define WATCHDOG_FEED_INTERVAL 10

int
main(int argc, char *argv[])
{
    while (1) {
        if (watchdog_enale() < 0) {
            sleep(3);
            continue;
        }

        while (watchdog_feed() >= 0)
            sleep(WATCHDOG_FEED_INTERVAL);
    }
    return 0;
}
