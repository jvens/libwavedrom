EXE=main
SRC=main.c ./json-builder/json-builder.c
LIB=./json-parser/libjsonparser.a
CFLAGS=-Wall -I./json-builder/ -I./json-parser/
LFLAGS=-lm

OBJ := $(patsubst %.c,%.o,$(SRC))

all: $(EXE)

$(EXE): $(OBJ) $(LIB)
	gcc -o $@ $(LFLAGS) $^

%.o: %.c
	gcc -o $@ -c $(CFLAGS) $<

clean:
	rm -f $(EXE)
	rm -f $(OBJ)

