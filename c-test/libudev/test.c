#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#include <linux/netlink.h>
#include <libudev.h>
#include <errno.h>

#define UEVENT_BUFFER_SIZE 2048

static int init_hotplug_sock(void)
{
	struct sockaddr_nl snl;
	const int buffersize = 16*1024*1024;
	int retval;

	memset(&snl, 0x00, sizeof(struct sockaddr_nl));
	snl.nl_family = AF_NETLINK;
	snl.nl_pid = getpid();
	snl.nl_groups = 1;
	int hotplug_sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
	if (hotplug_sock == -1) {
		printf("error getting socket: %s\n", strerror(errno));
		return -1;
	}

	setsockopt(hotplug_sock, SOL_SOCKET, SO_RCVBUFFORCE, &buffersize,
		   sizeof(buffersize));
	retval = bind(hotplug_sock, (struct sockaddr *)&snl, sizeof(struct sockaddr_nl));
	if (retval < 0) {
		printf("bind failed: %s\n", strerror(errno));
		close(hotplug_sock);
		hotplug_sock = -1;
		return -1;
	}

	return hotplug_sock;
}


int main(int argc, char *argv[])
{
	int hotplug_sock = init_hotplug_sock();
	struct udev *udev;
	struct udev_monitor *mon;
	
	struct udv_device *dev;
	const char *path;
	const char *dev_node;
	const char *action;
		

	udev = udev_new();
	if (udev == NULL) {
		printf("new udev error!\n");
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
		printf("uenum create failed.\n");
		return -1;
	}
	if (udev_enumerate_add_match_subsystem(uenum, "block") < 0) {
		printf("uenum add match subsystem failed.\n");
		return -1;
	}
	if (udev_enumerate_scan_devices(uenum) < 0) {
		printf ("uenum scan devices failed.\n");
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

		printf("=====enume  path:%s, node:%s\n", path, dev_node);
	}
	
	
	
	while (1) {
		
		dev = NULL;
		dev = udev_monitor_receive_device(mon);
		if (dev == NULL)
			continue;
		path = udev_device_get_devpath(dev);
		dev_node = udev_device_get_devnode(dev);
		action = udev_device_get_action(dev);
		printf("path: %s, devnode: %s, event: %s\n",
		       path, dev_node, action);
		
	}
	return 0;
}

