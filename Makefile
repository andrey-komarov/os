#-include Makefile.local

export TARGET = i686-elf

export CC = $(TARGET)-gcc
export CFLAGS = -Wall -Wextra -Wpedantic -std=c99 -nostdlib -ffreestanding -I.. -I. -lgcc -g
export LIBGCC = $(shell $(CC) -print-libgcc-file-name)

export AS = $(TARGET)-as
export ASFLAGS = 

export LD=$(TARGET)-ld
export LDFLAGS = -T config/linker.ld

export AR = $(TARGET)-ar
export ARFLAGS = rcs

QEMU ?= qemu-system-i386
QEMUFLAGS ?= -m 256 -boot d

GRUB_MKRESCUE ?= grub-mkrescue
ISO_ROOT ?= isoroot
ISO ?= kernel.iso
OUTPUT_DIR ?= $(ISO_ROOT)/boot
KERNEL ?= $(OUTPUT_DIR)/kernel.bin
DISK ?= hda.bin

SUBMODULES ?= boot mem kernel tty ata fs libc
GRUB_CONF ?= config/grub.cfg

OBJ = $(foreach DIR, $(SUBMODULES), $(DIR)/$(DIR).a)

run: $(ISO) $(DISK)
	$(QEMU) $(QEMUFLAGS) -cdrom $(ISO) -hda $(DISK)

$(ISO): $(KERNEL)
	-mkdir -p $(OUTPUT_DIR)/grub
	cp $(GRUB_CONF) $(OUTPUT_DIR)/grub
	$(GRUB_MKRESCUE) -o $(ISO) $(ISO_ROOT)

$(KERNEL): $(OBJ)
	-mkdir -p $(OUTPUT_DIR)
	$(LD) $(LDFLAGS) -\( $^ -\) $(LIBGCC) -o $@

$(DISK):
	dd if=/dev/zero of=$(DISK) count=32000
	mkfs.fat -F 16 $(DISK)
	echo "Hello FAT" > hello.txt
	mcopy -i $(DISK) hello.txt ::hello.txt

%.a: FORCE
	@echo "Building $(shell dirname $@)"
	@$(MAKE) --no-print-directory -C $(shell dirname $@)

FORCE:

clean:
	@for dir in $(SUBMODULES) ; do \
		echo "Cleaning $$dir"; \
		$(MAKE) --no-print-directory clean -C $$dir; \
	done
	rm -rf $(ISO) $(ISO_ROOT)

.PHONY: all clean $(ISO)
