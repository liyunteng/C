#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#include <linux/netlink.h>
#include <libudev.h>
#include <errno.h>

#define UEVNET_BUFFER_SIZE 2048

int main(int argc, char *argv[])
{
	struct udev *udev;
	struct udev_monitor *mon;
	struct udev_device *dev;
	const char *path;
	const char *dev_node;
	const char *action;

	udev = udev_new();
	if (udev == NULL) {
		printf("new uedv error!\n");
		return -1;
	}

	mon = udev_monitor_new_from_netlink(udev, "udev");
	if (mon == NULL) {
		printf("create mon error!\n");
		return -1;
	}

	if (udev_monitor_filter_add_match_subsystem_devtype(mon,
							    "block",
							    "disk") < 0) {
		printf("udev monitor add match failed!\n");
		udev_unref(udev);
		return -1;
	}

	udev_monitor_set_receive_buffer_size(mon, 128*1024);
	if (udev_monitor_enable_receiving(mon) < 0) {
		printf("monitor enable failed!\n");
		udev_monitor_unref(mon);
		udev_unref(udev);
		return -1;
	}

	struct udev_enumerate *uenum;
	struct udev_list_entry *devs, *dev_list;
	uenum = udev_enumerate_new(udev);
	if (uenum == NULL) {
		printf("uenum create failed.");
		return -1;
	}

	if (udev_enumerate_add_match_subsystem(uenum, "block") < 0) {
		printf("uenum add match subsystem failed.\n");
		return -1;
	}

	if (udev_enumerate_scan_devices(uenum) < 0) {
		printf("uenum scan devices failed.\n");
		return -1;
	}

	devs = udev_enumerate_get_list_entry(uenum);
	udev_list_entry_foreach(dev_list, devs) {

		path = udev_list_entry_get_name(dev_list);
		if (path == NULL)
			continue;
		dev = udev_device_new_from_syspath(udev, path);
		if (dev == NULL)
			continue;

		dev_node = udev_device_get_devnode(dev);
		if (dev_node == NULL)
			continue;

		printf("====enume path:%s, node:%s\n", path, dev_node);
	}
	

	while (1) {
		dev = NULL;
		dev = udev_monitor_receive_device(mon);
		if (dev == NULL)
			continue;
		path = udev_device_get_devpath(dev);
		dev_node = udev_device_get_devnode(dev);
		action = udev_device_get_action(dev);
		printf("path: %s, devnode:%s, event: %s\n",
		       path, dev_node, action);
	}
	return 0;
}


