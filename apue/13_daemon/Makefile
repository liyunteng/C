OBJECTS := daemonize.o daemon_reconfig.o already_running.o
CFLAGS += -Wall
LIBS += -lpthread

all:$(OBJECTS) daemon_reconfig

daemon_reconfig : $(OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONE : clean
clean:
	rm -rf $(OBJECTS) daemon_reconfig
