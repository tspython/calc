CC=gcc
CFLAGS=-Iinclude -Ilib/tinyexpr
LDFLAGS=-lraylib -lGL -lm -lpthread -ldl -lrt -lX11

SRC=$(wildcard src/*.c)
OBJ=$(SRC:.c=.o)

graphcalc: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f src/*.o graphcalc

