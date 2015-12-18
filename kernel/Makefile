OUTPUT = kernel.a
OBJ = kernel.o interrupts.o gdt.o handler.o ioports.o pic.o mmu.o tss.o

all: $(OUTPUT)

$(OUTPUT): $(OBJ)
	$(AR) $(ARFLAGS) $@ $^

clean:
	rm -rf $(OBJ) $(OUTPUT)
