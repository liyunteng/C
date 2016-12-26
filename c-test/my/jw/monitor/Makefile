CFLAGS += -Wstrict-aliasing=0
CFLAGS += -I/usr/include/libxml
CFLAGS += -std=c99
CFLAGS += -D_DEBUG

LDFLAGS += libsysmon.a -lxml2 -ljson -lev -lsqlite3

OBJS= debug.o sys-tmpfs.o sys-global.o sys-module.o sys-action.o sys-alarm.o sys-conf.o \
     sys-event.o list.o sys-interval-check.o sys-capture.o pmu-info.o

all: daemon libsysmon.a
ifneq ($(STRIP),)
	$(STRIP) sys-mon
endif

daemon: $(OBJS) libsysmon.a
	gcc -o sys-mon sys-mon.c $(OBJS) $(CFLAGS) $(LDFLAGS)

libsysmon.a:
	gcc -c libsysmon.c
	ar -rcs libsysmon.a libsysmon.o

clean:
	rm -rf sys-mon core $(OBJS) libsysmon.a buzzer
