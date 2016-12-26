obj-m := my_hello.o

PWD := $(shell pwd)
KERNELDIR := /lib/modules/test/build

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD)

clean:
	rm -rf *.ko *.o
	rm -rf *.cmd
	rm -rf *.cmd.c

