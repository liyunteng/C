obj-m := my_bus.o my_dev.o my_driver.o
PWD := $(shell pwd)
KERNELDIR := /lib/modules/$(shell uname -r)/build

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD)
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
