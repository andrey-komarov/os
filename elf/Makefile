export CFLAGS=-m32 -T linker.ld
export LDFLAGS=-m32 -T linker.ld

all: loader

loader: loader.o

clean:
	rm -f *.o loader

.PHONY: all clean
