OBJECTS := test_1.o test_abort.o test_fork.o test_jmp.o test_lock.o \
	test_rlimit.o test_uname.o test_vfork.o
CFLAGS += -Wall
LIBS +=
TARGETS := test_1 test_abort test_fork test_jmp test_lock test_rlimit \
	test_uname test_vfork

all: $(TARGETS)

test_1: test_1.o
test_abort: test_abort.o
test_fork: test_fork.o
test_jmp: test_jmp.o
test_lock: test_lock.o
test_rlimit: test_rlimit.o
test_uname: test_uname.o
test_vfork: test_vfork.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONE: clean
clean:
	rm -rf $(OBJECTS) $(TARGETS)
