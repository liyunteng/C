#include <parted/parted.h>
#include <stdio.h>

int
main(int argc, char *argv[])
{
    ped_device_probe_all();
    PedDevice *   dev  = 0;
    PedDisk *     disk = NULL;
    PedDiskType * type;
    PedPartition *part;

    while ((dev = ped_device_get_next(dev))) {
        printf("\n====================\n\n");
        printf("device model: %s\n", dev->model);
        printf("path: %s\n", dev->path);

        long long size = (dev->sector_size * dev->length) / (1024 * 1024 * 1024);
        printf("size: %llu G\n", size);

        if (!(type = ped_disk_probe(dev))) {
            printf("get type failed.\n");
            continue;
        }
        printf("type: %s\n", type->name);
        disk = ped_disk_new(dev);
        if (disk) {
            printf("needs_clobber: %d\n", disk->needs_clobber);
            printf("update_mode: %d\n", disk->update_mode);
            printf("total patition: %d\n", ped_disk_get_last_partition_num(disk));
        } else {
            printf("disk is NULL.\n");
            continue;
        }

        for (part = ped_disk_next_partition(disk, NULL); part;
             part = ped_disk_next_partition(disk, part)) {
            printf("--------------------\n");
            if (part->type & PED_PARTITION_METADATA) {
                printf("metadata area.\n");
                continue;
            }
            if (part->type & PED_PARTITION_FREESPACE) {
                printf("free space.\n");
                continue;
            }

            printf("partition path: %s\n", ped_partition_get_path(part));
            printf("partition num: %d\n", part->num);
            printf("partition start: %llu\n", part->geom.start);
            printf("partition end: %llu\n", part->geom.end);
            printf("partition length: %lld\n", part->geom.length);
        }

        ped_disk_destroy(disk);
    }
    return 0;
}

/* Local Variables: */
/* compile-command: "clang -Wall -o test test.c -g -lparted" */
/* End: */
