obj-m := memdev.o
PWD := $(shell pwd)
KERNELDIR := /lib/modules/$(shell uname -r)/build

all:
	echo $(KBUILD_CFLAGS)
	$(MAKE) -C $(KERNELDIR) M=$(PWD)
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
