obj-m := kset_test.o
PWD := $(shell pwd)
KERNELDIR := /lib/modules/$(shell uname -r)/build

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD)
clean:
	rm -rf *.ko *.o  .*.cmd  .tmp_versions *.symvers *.order


