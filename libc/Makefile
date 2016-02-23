OUTPUT = libc.a
OBJ = string.o stdio.o panic.o bitset.o

all: $(OUTPUT)

$(OUTPUT): $(OBJ)
	$(AR) $(ARFLAGS) $@ $^

clean:
	rm -rf $(OBJ) $(OUTPUT)

.PHONY: all clean
