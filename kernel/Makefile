OUTPUT = kernel.a
OBJ = kernel.o interrupts.o gdt.o ioports.o pic.o mmu.o tss.o userspace.o isr.o handlers.o loader.o

all: $(OUTPUT)

$(OUTPUT): $(OBJ)
	$(AR) $(ARFLAGS) $@ $^

clean:
	rm -rf $(OBJ) $(OUTPUT)
