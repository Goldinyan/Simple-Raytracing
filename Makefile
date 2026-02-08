CC = clang

CFLAGS = -Wall -Wextra -std=c11 -Iinclude -I/opt/homebrew/include \
         -fsanitize=address -g -O0

CFLAGS_PERF = -Wall -Wextra -std=c11 -Iinclude -I/opt/homebrew/include -O3

LDFLAGS = -fsanitize=address -L/opt/homebrew/lib -lSDL2

LDFLAGS_PERF = -L/opt/homebrew/lib -lSDL2

SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, object_files/%.o, $(SRC))
OBJ_PERF = $(patsubst src/%.c, object_files/perf/%.o, $(SRC))

TARGET = program
TARGET_PERF = program_perf

all: $(TARGET)

perf: $(TARGET_PERF)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

$(TARGET_PERF): $(OBJ_PERF)
	$(CC) $(OBJ_PERF) -o $(TARGET_PERF) $(LDFLAGS_PERF)

object_files/%.o: src/%.c
	@mkdir -p object_files
	$(CC) $(CFLAGS) -c $< -o $@

object_files/perf/%.o: src/%.c
	@mkdir -p object_files/perf
	$(CC) $(CFLAGS_PERF) -c $< -o $@

clean:
	rm -f object_files/*.o object_files/perf/*.o $(TARGET) $(TARGET_PERF)

