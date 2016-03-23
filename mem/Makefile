OUTPUT = mem.a
OBJ = mem.o kpmalloc.o context.o

all: $(OUTPUT)

$(OUTPUT): $(OBJ)
	$(AR) $(ARFLAGS) $@ $^

clean:
	rm -rf $(OBJ) $(OUTPUT)

.PHONY: all clean
