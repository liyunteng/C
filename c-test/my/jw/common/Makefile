CC=gcc
CFLAGS+=-DNDEBUG
OBJECTS=log.o log-utils.o debug.o mkdir-p.o

all: liblog.a daemon

liblog.a: $(OBJECTS)
	ar -rcs liblog.a $(OBJECTS)

daemon: log-daemon.c
	gcc $(CARGS) -o log-daemon log-daemon.c liblog.a -lsqlite3

ifneq ($(STRIP),)
	$(STRIP) log-daemon
endif

clean:
	rm -rf liblog.a $(OBJECTS) log-daemon

