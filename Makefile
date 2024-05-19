CC=clang
# Include the specific path to the headers
CFLAGS=-I/opt/homebrew/Cellar/raylib/5.0/include -Iinclude -Ilib/tinyexpr -Ilib/raygui/src
# Linking directories
LDFLAGS=-L/opt/homebrew/Cellar/raylib/5.0/lib -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
# Linking libraries
LDLIBS=-lraylib

# Source files including tinyexpr.c
SRC=$(wildcard src/*.c) lib/raygui/src/raygui.c lib/tinyexpr/tinyexpr.c
# Object files
OBJ=$(SRC:.c=.o)

graphcalc: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f src/*.o lib/raygui/src/*.o lib/tinyexpr/*.o graphcalc

