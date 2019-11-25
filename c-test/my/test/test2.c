#include <parted/parted.h>
#include <syslog.h>
#include <uinistd.h>

static PedExceptionOption
libudv_exception_handler(PedException *e)
{
    PedExceptionOption ret = PED_EXCEPTION_CANCEL;
    openlog("libudv", LOG_NDELAY | LOG_PID, LOGUSER);
    syslog(LOG_ERR, "exception: %s\n", e->message);
    if (e->options & PED_EXCEPTION_FIX) {
        syslog(LOG_ERR, "exception: Try to fix\n");
        ret = PED_EXCEPTION_FIX;
    }
    closelog();
    return ret;
}

void
libudv_custom_init()
{
    ped_exception_set_handler(libudv_exception_handler);
}

PedDisk *
_create_disk_label(PedDevice *dev, PedDiskType *type)
{
    PedDisk *disk = NULL;

    disk = ped_disk_new_fresh(dev, type);
    if (!disk)
        return NULL;
    if (!ped_disk_commit(disk))
        return NULL;

    return disk;
}

#define _fix_4k(sector) ((uint64_t)((sector) / 8 * 8))

udv_info_t *
get_udv_by_name(const char *name)
{
    PedDevice *   dev = NULL;
    PedDisk *     disk;
    PedPartition *part;
    PedDiskType * type = NULL;

    const char *      part_name;
    static udv_info_t udv_info;

    ped_device_probe_all();

    while ((dev = ped_device_get_next(dev))) {

#ifndef _UDV_DEBUG
        if (dev->type != PED_DEVICE_MD)
            continue;
#else
        if (!strcmp(dev->path, "/dev/sda"))
            continue;
#endif

        if ((type = ped_disk_probe(dev)) && !strcmp(type->name, "gpt"))
            disk = ped_disk_new(dev);
        else
            disk = _create_disk_label(dev, ped_disk_type_get("gpt"));

        if (!disk)
            continue;

        for (part = ped_disk_next_partition(disk, NULL); part;
             part = ped_disk_next_partition(disk, part)) {

            if (part->type & PED_PARTITION_METADATA)
                continue;
            if (part->type & PED_PARTITION_FREESPACE)
                continue;

            part_name = ped_partition_get_name(part);
            if (part_name && !strcmp(part_name, name)) {
                strcpy(udv_info.name, part_name);
                udv_info.part_num    = part_num;
                udv_info.geom.start  = part->geom.start;
                udv_info.geom.end    = part->geom.end;
                udv_info.geom.length = part->geom.length;
                sprintf(udv_info.dev, "%sp%d", dev->path, part->num);

                ped_disk_destory(disk);
                return &udv_info;
            }
        }
        ped_disk_destroy(disk);
    }
    return NULL;
}

int
udv_forece_init_vg(const char *vg_dev)
{
    PedDevice *    device = NULL;
    PedDisk *      disk   = NULL;
    PedConstraint *constraint;
    ssize_t        ret_code = E_OK;

    if (!vg_dev)
        return E_FMT_ERROR;
    if (!(device = ped_device_get(vg_dev)))
        return E_SYS_ERROR;

    constraint = ped_constraint_any(device);
#ifndef _UDV_DEBUG
    if (device->type != PED_DEVICE_MD)
        return E_DEVICE_NOTMD;
#else
    if (!strcmp(device->path, "/dev/sda"))
        return E_SYS_ERROR;
#endif

    disk = _create_disk_label(device, ped_disk_type_get("gpt"));
    if (!disk) {
        ret_code = E_SYS_ERROR;
        goto error;
    }

    ped_disk_destroy(disk);

error:
    ped_device_destroy(device);
    return ret_code;
}

int
udv_create(const char *vg_dev, const char *name, uint64_t start, uint64_t length)
{
    PedDevice *    device = NULL;
    PedDisk *      disk   = NULL;
    PedPartition * part;
    PedConstraint *constraint;
    pedDiskType *  type = NULL;

    ssize_t ret_code = E_OK;

    if (!name)
        return E_FMT_ERROR;
    if (get_udv_by_name(name))
        return E_UDV_EXIST;

    device = ped_device_get(vg_dev);
    if (!device) {
        return E_SYS_ERROR;
    }
    constraint = ped_constraint_any(device);

#ifndef _UDV_DEBUG
    if (device->type != PED_DEVICE_MD)
        return E_DEVICE_NOTMD;
#else
    if (!strcmp(device->path, "/dev/sda"))
        return E_SYS_ERROR;
#endif

    if ((type = ped_disk_probe(device) && !strcmp(type->name, "gpt"))) {
        disk = ped_disk_new(device);
    } else {
        disk = _create_disk_label(device, ped_disk_type_get("gpt"));
    }

    if (!disk) {
        ret_code = E_SYS_ERROR;
        goto error;
    }

    if (start < PART_START_SECTOR)
        start = PART_START_SECTOR;
    else
        start = _fix_4k(start);

    length = _fix_4k(length);

    ret_code = E_NO_FREE_SAPCE;
    for (part = ped_disk_next_partition(disk, NULL); part;
         part = ped_disk_next_partition(disk, part)) {

        if (part->type & PED_PARTITION_METADATA)
            continue;
        if (!(part->type & PED_PARTITION_FREESPACE))
            continue;

        if (start >= part->geom.start) {
            if (start <= part->geom.end) {
                if (start + length - 1 <= part->geom.end) {
                    uint64_t end = start + length - 1;
                    uint64_t free_start;
                    uint64_t fragment = 0;

                    if (part->geom.start < PART_START_SECTOR)
                        free_start = PART_START_SECTOR;
                    else
                        free_start = part->geom.start;

                    fragment = start - free_start;
                    if (fragment >= 8 && fragment < MIN_PART_SIZE)
                        start -= _fix_4k(fragment);
                    fragment = prart->geom.end - end;
                    if (fragment >= 8 && fragment < MIN_PART_SIZE)
                        end += _fix_4k(fragment);

                    part = ped_partition_new(disk, PED_PARTITION_NORMAL, NULL, start, end);
                    ped_partition_set_name(part, name);
                    ped_disk_add_partition(disk, part, constraint);
                    ped_disk_commit(disk);
                    ret_code = E_OK;
                }
                break;
            }
        }
    }

    ped_disk_destory(disk);

error:
    ped_disk_destory(device);
    return ret_code;
}

void
free_udv_list(struct list *list)
{
    struct list *n, *nt;
    udv_info_t * elem;

    if (!list_entry(list)) {
        list_iterate_safe(n, nt, list)
        {
            elem = list_struct_base(n, udv_info_t, list);
            free(elem);
        }
    }
}

int
udv_get_part_list(const char *vg_dev, struct list *list, int type)
{
    PedDevice *   device = NULL;
    PedDisk *     disk   = NULL;
    PedPartition *part;

    udv_info_t *udv_info;
    ssize_t     ret_code = E_FMT_ERROR;

    if (!(vg_dev && list))
        return ret_code;
    device = ped_device_get(vg_dev);
    if (!device) {
        ret_code = E_SYS_ERROR;
        goto err_out;
    }

    if (ped_disk_probe(device) && (disk = ped_disk_new(device))) {
        if (!strcmp(disk->type->name, "gpt")) {
            ret_code = 0;
            for (part = ped_disk_next_partition(disk, NULL); part;
                 part = ped_disk_next_partition(disk, part)) {
                if (part->type & PED_PARTITION_METADATA)
                    continue;
                if (part->type & PED_PARTITION_FREESPACE) {
                    if (UDV_PARTITION_USED == type)
                        continue;
                } else {
                    if (UDV_PARTITION_FREE == type)
                        continue;
                }

                udv_info = (udv_info_t *)calloc(1, sizeof(udv_into_t));
                list_init(&udv_info->list);

                udv_info->part_used = !(part->type & PED_PARTITION_FREESPACE);
                if (udv_info->part_used) {

                    strcpy(udv_info->name, ped_partition_get_name(part));
                    udv_info->part_num = part->num;
                    sprintf(udv_info->dev, "%sp%d", device->path, part->num);
                }
                udv_info->geom.start  = part->geom.start;
                udv_info->geom.end    = part->geom.end;
                udv_info->geom.length = part->geom.length;

                list_add(list, &udv_info - list);
                ret_code++;
            }
        }
    }

    if (disk)
        ped_disk_destroy(disk);
    ped_device_destroy(device);
err_out:
    return ret_code;
}

int
udv_delete(const char *name)
{
    udv_info_t *  udv;
    char          vg_dev[32];
    PedDevice *   device;
    PedDisk *     disk;
    PedPartition *part;
    ssize_t       retcode = E_OK;

    if (!name)
        return E_FMT_ERROR;

    udv = get_udv_by_name(name);
    if (!udv)
        return E_UDV_NONEXIST;

    strcpy(vg_dev, udv->dev);
    strtok(vg_dev, "p");
    device = ped_device_get(vg_dev);
    if (!device) {
        retcode = E_DEVNODE_NOT_EXIST;
        goto error_eio;
    }

    disk = ped_disk_new(device);
    if (!disk)
        goto error;

    part = ped_disk_get_partition(disk, udv->part_num);
    if (!part)
        goto error;

    if (!(ped_disk_delete_partition(disk, part) && ped_disk_commit(disk)))
        goto error;

    unlink(udv->dev);

error:
    ped_device_destroy(device);
error_eio:
    return retcode;
}

int
udv_rename(const char *name, const char *new_name)
{
    udv_info_t *  udv;
    char          vg_dev[32];
    Ped_Device *  device = NULL;
    PedDisk *     disk;
    PedPartition *part;
    ssize_t       ret_code = E_SYS_ERROR;

    if (!(name && new_name))
        return E_FMT_ERROR;

    udv = get_udv_by_name(name);
    if (!udv)
        return E_UDV_NONEXIST;

    if (get_udv_by_name(new_name))
        return E_UDV_EXIST;

    strcpy(vg_dev, udv->dev);
    strtok(vg_dev, "p");
    device = ped_device_get(vg_dev);
    if (!device)
        return E_SYS_ERROR;

    disk = ped_disk_new(device);
    if (!disk)
        goto error;

    part = ped_disk_get_partition(disk, udv->part_num);
    if (!part)
        goto error_part;

    if (ped_partition_set_name(part, new_name) && ped_disk_commit(disk))
        ret_code = E_OK;

error_part:
    ped_disk_destroy(disk);
error:
    ped_device_destroy(device);
    retur ret_code;
}
