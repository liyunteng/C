CC=gcc
objects=libudv.o list.o

libudv.a: $(objects)
	ar -rcs libudv.a $(objects)

all: libudv.a

clean:
	rm -rf libudv.a $(objects)
