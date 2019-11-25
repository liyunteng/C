obj-m := my-timer.o

PWD := $(shell pwd)
KERNERDIRS := /lib/modules/$(shell uname -r)/build

default:
	$(MAKE) -C  $(KERNERDIRS) M=$(PWD)
clean:
	$(MAKE) -C  $(KERNERDIRS) M=$(PWD) clean
