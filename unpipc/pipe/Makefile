CFLAGS += -Wall -O2 -I../
LDFLAGS += -lpthread
STRIP = strip
ifdef debug
	STRIP =
	CFLAGS = -Wall -g -DDEBUG 
endif

SRC = $(wildcard *.c)
OBJECTS = $(SRC:%.c=%.o)
DEP = $(SRC:%.c=%.d)
TARGETS = $(filter-out comm,$(OBJECTS:%.o=%))

all : $(TARGETS)
$(TARGETS) : comm.o
$(TARGETS) : % : %.o
	$(CC) $(CFLAGS) -o $@ $^  $(LDFLAGS)
ifneq ($(STRIP),)
	$(STRIP) $@
endif

%.d : %.c
	@set -e; rm -rf $@;\
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $;$@; \
	rm -rf $@.$$$$
-include $(DEP)

.PHONY: clean 
clean:
	@echo "cleanning..."
	@rm -rf $(OBJECTS) $(TARGETS) $(DEP)
