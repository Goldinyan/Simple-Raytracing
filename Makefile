CC = clang

CFLAGS = -Wall -Wextra -std=c11 -Iinclude -I/opt/homebrew/include \
         -fsanitize=address -g -O0

LDFLAGS = -fsanitize=address -L/opt/homebrew/lib -lSDL2

SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, object_files/%.o, $(SRC))

TARGET = program

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

object_files/%.o: src/%.c
	@mkdir -p object_files
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f object_files/*.o $(TARGET)

