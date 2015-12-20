
export TARGET = i686-elf

export CC=$(TARGET)-gcc
export CFLAGS = -Wall -Wextra -Wpedantic -std=c99 -nostdlib -ffreestanding -I../stdlib -lgcc -g

export AS = $(TARGET)-as

export LD=$(TARGET)-ld

all: hello argv

hello: hello.o
	$(LD) $< -o $@

hello.o: hello.S
	$(AS) $< -o $@

argv: argv.o
	$(LD) $< ../stdlib/stdlib.a -o $@

clean:
	rm -f *.o hello argv

.PHONY: all clean
