#ifndef VSD_WARNING__H__
#define VSD_WARNING__H__

#define NETLINK_VSD 31

#define WARNING_AREA_SUPER 0
#define WARNING_AREA_SECT_MAP 1
#define WARNING_AREA_NEW_SECT 2
#define WARNING_AREA_BAD_SECT 3

#define WARNING_LEVEL_1 1
#define WARNING_LEVEL_2 2
#define WARNING_LEVEL_3 3

struct vsd_warning_info {
    char disk_name[8];
    unsigned long warning_area;
    unsigned int mapped_cnt;
    unsigned int max_map_cnt;
};
#endif  // VSD_WARNING__H__
