#ifndef PIC_CTL_H
#define PIC_CTL_H

#include "pic_reg.h"
#include <stdint.h>

#define HZ (16)

enum {
    PERR_SUCCESS = 0,
    PERR_NODEV   = -1,
    PERR_IOERR   = -2,
    PERR_INVAL   = -3,
};

enum {
    PIC_LED_NUMBER = 16,

    PIC_LED_OFF   = 0x00,
    PIC_LED_ON    = 0x01,
    PIC_LED_BLINK = 0x02,
    PIC_LED_B2    = 0x03,

    PIC_LED_FREQ_NORMAL = PIC_HZ / 2,
    PIC_LED_FREQ_SLOW   = PIC_HZ,
    PIC_LED_FREQ_FAST   = PIC_HZ / 8,

    PIC_LED_STS_MASK   = 0x03,
    PIC_LED_FREQ_SHIFT = 2,
    PIC_LED_FREQ_MASK  = 0xfc,

};

enum {
    PIC_WDT_START = 0x55,
    PIC_WDT_STOP  = 0xaa,
};

int pic_init(void);
void pic_release(void);
int pic_get_version(uint32_t *version);
int pic_set_led(uint8_t led, uint8_t sts, uint8_t freq);
int pic_start_watchdog(void);
int pic_stop_watchdog(void);

#endif
