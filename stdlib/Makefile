export TARGET=i686-elf

export CC=$(TARGET)-gcc
export CFLAGS = -Wall -Wextra -Wpedantic -std=c99 -nostdlib -ffreestanding -lgcc -g

export AS=$(TARGET)-as

export LD=$(TARGET)-ld

export AR=$(TARGET)-ar
export ARFLAGS=rcs

OUTPUT=stdlib.a
OBJ=syscall.o unistd.o start.o

all: $(OUTPUT)

$(OUTPUT): $(OBJ)
	$(AR) $(ARFLAGS) $@ $^

clean:
	rm -f *.o *.a

.PHONY: all clean
