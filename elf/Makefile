export TARGET = i686-elf

export CC = $(TARGET)-gcc
export CFLAGS = -Wall -Wextra -Wpedantic -std=c99 -nostdlib -ffreestanding -I../stdlib -I. -lgcc -g
export LIBGCC = $(shell $(CC) -print-libgcc-file-name)

export LD=$(TARGET)-ld
export LDFLAGS=-T linker.ld
export LDPATH=../stdlib/stdlib.a $(LIBGCC)

all: loader

loader: loader.o
	$(LD) $(LDFLAGS) $< $(LDPATH) -o $@

clean:
	rm -f *.o loader

.PHONY: all clean
