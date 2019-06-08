CC := clang
CPPFLAGS = -I../include
CFLAGS = -fPIC
LDFLAGS =

SRC=$(wildcard *.c)
OBJECTS=$(SRC:%.c=%.o)
DEP = $(SRC:%.c=%.d)
TARGET = libusrhdr.so

all: $(TARGET)

$(filter %.so,$(TARGET)): $(OBJECTS)
	$(CC) $(CFLAGS) -shared $(CFLAGS) -o $@ $^

%.d: %.c
	@set -e;rm -rf $@; \
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
    sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
    rm -rf $@.$$$$

-include $(DEP)

.PHONY: clean
clean:
	rm -rf $(OBJECTS) $(DEP)
	rm -rf $(TARGET)
