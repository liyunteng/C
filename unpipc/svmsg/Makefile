#OBJECTS := ctl.o msgcreate.o msgsnd.o msgrcv.o msgrm.o client_main.o \
server_main.o comm.o server.o client.o
CFLAGS += -Wall
LIBS +=
SRC := ctl.c msgcreate.c msgsnd.c msgrcv.c msgrm.c client_main.c \
server_main.c server.c client.c
HEAD := comm.h svmsg.h
#TARGETS := ctl msgcreate msgsnd msgrcv msgrm client_main server_main \
server client
TARGETS_OBS := $(SRC:%.c=%.o)
TARGETS := $(SRC:%.c=%)

all:  $(TARGETS)

$(TARGETS_OBS):%.o:%.c
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

client_main: client_main.o comm.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
server_main: server_main.o comm.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
server : server.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
client : client.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY:clean
clean:
	rm -rf  *.o

