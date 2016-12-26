obj-m := nl_ker.o
KDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	gcc nl_usr.c -o nl_usr
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	rm -rf nl_usr
	$(MAKE) -C $(KDIR) M=$(PWD) clean
