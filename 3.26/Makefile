GCC = gcc
EXE	= string_reverse
OBJ	= string_reverse.o
SOURCE = string_reverse.c

default: $(EXE)

$(OBJ): $(SOURCE)
	$(GCC) -c -o $@ $(SOURCE) -std=gnu99 -lrt

$(EXE): $(OBJ)
	$(GCC) $(OBJ) -o $(EXE) -std=gnu99 -lrt

clean:
	rm -rf *.o $(EXE)