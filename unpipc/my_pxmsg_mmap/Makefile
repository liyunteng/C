OBJECTS := libmq.o mqunlink.o mqgetattr.o mqsetattr.o mqcreate.o \
	mqsend.o mqreceive.o mqnotifysig5.o mqnotifythread1.o
CFLAGS += -Wall -g
LIBS += -lpthread 
TARGETS := mqcreate mqgetattr  mqnotifysig5  \
	mqnotifythread1  mqsend mqreceive mqunlink libmq.so libmq.a

all: $(TARGETS)
libmq.a : libmq.o libmq.o
	ar -rcs $@ $^
libmq.so : libmq.c libmq.h
	$(CC) -o $@ -fPIC -shared $^ $(CFLAGS) -lpthread
mqunlink : mqunlink.o libmq.a
	$(CC) -o $@ $^ $(LIBS) $(CFLAGS)
mqgetattr : mqgetattr.o libmq.a
	$(CC) -o $@ $^ $(LIBS) $(CFLAGS)
mqcreate : mqcreate.o libmq.a
	$(CC) -o $@ $^ $(LIBS) $(CFLAGS)
mqsend : mqsend.o libmq.a
	$(CC) -o $@ $^ $(LIBS) $(CFLAGS)
mqreceive : mqreceive.o libmq.a
	$(CC) -o $@ $^ $(LIBS) $(CFLAGS)
mqnotifysig5 : mqnotifysig5.o libmq.a
	$(CC) -o $@ $^ $(LIBS) $(CFLAGS)
mqnotifythread1 : mqnotifythread1.o libmq.a
	$(CC) -o $@ $^ $(LIBS) $(CFLAGS)


.PHONE: clean
clean:
	rm -rf $(OBJECTS) $(TARGETS) *.o
