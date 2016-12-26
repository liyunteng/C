#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <parted/parted.h>

#define _fix_4k(sector) ((uint64_t)((sector)/8*8))
int main(int argc, char *argv[])
{
    PedDevice *device = NULL;
    PedDisk *disk = NULL;
    PedPartition *part;
    PedConstraint *constraint = NULL;
    PedDiskType *type = NULL;


    device = ped_device_get("/dev/sda");
    if (!device) {
	printf("device_get failed.\n");
	return -1;
    }
    constraint = ped_constraint_any(device);

    if ((type = ped_disk_probe(device))) {
	disk = ped_disk_new(device);
    } else {
	printf("error!\n");
	return -1;
    }

    if (!disk) {
	printf("new disk failed.\n");
	ped_device_destroy(device);
	return -1;
    }

    for (part = ped_disk_next_partition(disk, NULL); part;
	 part = ped_disk_next_partition(disk, part)) {
	if (part->type & PED_PARTITION_FREESPACE) {
	    uint64_t start, end;
	    start = _fix_4k(part->geom.start + 16);
	    end = _fix_4k(part->geom.end);
	    if (end <= start)
		continue;
	    printf("start: %lu, end: %lu\n", start, end);
	    printf("geom.start: %lu, end: %lu\n",
		   (uint64_t) part->geom.start, (uint64_t) part->geom.end);
	    if (part->geom.end >= end && part->geom.start <= start) {

		part = ped_partition_new(disk, PED_PARTITION_LOGICAL,
					 ped_file_system_type_get("ext2"),
					 start, end);
		if (!part) {
		    printf("create part failed.\n");
		    ped_device_destroy(device);
		    return -1;
		}
//                              ped_partition_set_name(part, "test");
//                              ped_disk_add_partition(disk,  part, constraint);
		ped_disk_commit(disk);
		ped_disk_destroy(disk);
		ped_device_destroy(device);
		return 0;
	    }
	} else {
	    continue;
	}
    }

    return 0;
}
