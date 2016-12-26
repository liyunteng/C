obj-m := my_jiffies.o

PWD := $(shell pwd)
KERNELDIR := /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules


clean:
	rm -rf *.ko *.o
