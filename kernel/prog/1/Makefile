obj-m := hello.o

PWD := $(shell pwd)
KERNELDIR := /lib/modules/$(shell uname -r)/build

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD)

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
