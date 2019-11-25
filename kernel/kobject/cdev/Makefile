obj-m := cdev.o
PWD := $(shell pwd)
KERNELDIR := /lib/modules/$(shell uname -r)/build

deafult:
	$(MAKE) -C $(KERNELDIR) M=$(PWD)
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
