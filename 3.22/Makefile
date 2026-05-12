
GCC         = gcc
EXE	    = collatz
OBJ	    = collatz.o
SOURCE      = collatz.c

default: $(EXE)

$(OBJ): $(SOURCE)
	$(GCC) -c -o $@ $(SOURCE) -std=gnu99 -lrt

$(EXE): $(OBJ)
	$(GCC) $(OBJ) -o $(EXE) -std=gnu99 -lrt

clean:
	rm -rf *.o $(EXE)
