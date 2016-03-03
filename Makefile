EXE=main
SRC=main.c 
LIB=./libjson/libjson.a
CFLAGS=-Wall -g
LFLAGS=-lm

OBJ := $(patsubst %.c,%.o,$(SRC))

all: $(EXE)

$(EXE): $(OBJ) $(LIB)
	gcc -o $@ $(LFLAGS) $^

%.o: %.c
	gcc -o $@ -c $(CFLAGS) $<

doc: $(SRC)
	doxygen

clean:
	rm -f $(EXE)
	rm -f $(OBJ)

