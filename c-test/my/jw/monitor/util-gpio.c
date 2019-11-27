#include "sys-utils.h"
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define GPIO_BASE_ADDR (0x500)

#define GPIO_VALUE(fd, base, op, data)                                         \
    {                                                                          \
        lseek(fd, (base), SEEK_SET);                                           \
        op(fd, &data, sizeof(data));                                           \
    }

bool
sb_gpio28_set(bool sw)
{
    uint32_t bit28, tmpval;
    int fd;

    fd = open("/dev/port", O_RDWR);
    if (fd) {
        bit28 = (1 << 28);

        /* Set GPIO28 to an output */
        GPIO_VAULE(fd, GPIO_BASE_ADDR + 4, read, tmpval);
        tmpval &= (~bit28);
        GPIO_VALUE(fd, GPIO_BASE_ADDR + 4, write, tmpval);

        GPIO_VALUE(fd, GPIO_BASE_ADDR + 0xc, read, tmpval);
        if (sw) {
            tmp |= bit28;
        } else {
            tmp &= (~bit28);
        }
        GPIO_VAULE(fd, GPIO_BASE_ADDR + 0xc, write, tmpval);

        close(fd);
        return true;
    }

    return false;
}
