#ifndef US_DISK__H__
#define US_DISK__H__

#include <sys/types.h>

void us_disk_update_slot(char *slot, const char *op);
void us_disk_name_to_slot(int fd, char *name);
void us_disk_slot_to_name(int fdk, char *slots);
void us_disk_dump(int fd, char *slot, int detail);

ssize_t disk_name2slot(const char *name, char *slot);

enum {
    DISK_UPDATE_SMART = (1 << 0),
    DISK_UPDATE_RAID  = (1 << 1),
    DISK_UPDATE_STATE = (1 << 2),

    DISK_UPDATE_ALL = 0xff,
};

#endif  // US_DISK__H__
