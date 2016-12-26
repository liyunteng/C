#ifndef __JW_WATCHDOG_H__
#define __JW_WATCHDOG_H__

int watchdog_enable(void);
int watchdog_disable(void);
int watchdog_set_timeout(unsigned char timeout);
int watchdog_feed(void);

#endif // __JW_WATCHDOG_H__
