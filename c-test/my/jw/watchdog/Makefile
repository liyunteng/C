CROSS_COMPILE =

all: libwatchdog.a

progs = watchdog
lib-objs := watchdog_lib.o
watchdog-objs = watchdog.o
watchdog-libs = libwatchdog.a

-include config.mk

libwatchdog.a: $(call to-progs, $(lib-objs))
	$(QUITE_AR)

$(OBJS) : %.o : %.c
	$(QUITE_CC)

.PHONY: clean test utils

clean::
	$(call QUITE_RM, *.o *.a *.so)
