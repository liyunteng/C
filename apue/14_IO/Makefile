OBJECTS := test_NONBLOCK.o lock_test.o test_filelock.o readn_writen.o \
	test_mmap.o test_select.o
CFLAGS += -Wall
LIBS +=
TARGETS := test_NONBLOCK test_filelock test_mmap test_select

all: $(OBJECTS) $(TARGETS)

test_NONBLOCK : test_NONBLOCK.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
test_filelock : test_filelock.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
test_mmap : test_mmap.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
test_select : test_select.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONE: clean
clean:
	rm -rf $(OBJECTS) $(TARGETS) tmplock a.out


