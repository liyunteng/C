CC := clang
CPPFLAGS = -I../include
CFLAGS =
LDFLAGS = -L../lib -lourhdr

SRC=$(wildcard *.c)
OBJECTS=$(SRC:%.c=%.o)
TARGET = $(SRC:%.c=%)

all: $(TARGET)


.PHONY: clean all
clean:
	rm -rf $(OBJECTS) $(DEP)
	rm -rf $(TARGET)
