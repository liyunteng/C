CFLAGS += -Wall
LIBS += -lpthread
OBJECTS := getenv_r.o getenv_r_v2.o test_getenv.o test_cancel.o \
test_pthread_atfork.o test_signo.o mkdetached_thread.o

all: test_getenv test_cancel test_pthread_atfork test_signo

test_getenv : test_getenv.o getenv_r.o getenv_r_v2.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
test_signo : test_signo.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
test_pthread_atfork : test_pthread_atfork.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
test_cancel : test_cancel.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONE : clean
clean:
	rm -rf $(OBJECTS) test_getenv test_cancel test_pthread_atfork test_signo

