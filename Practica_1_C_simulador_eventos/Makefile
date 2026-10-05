CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -Werror -Iinclude
DEBUG_FLAGS := -g3 -O0
SAN_FLAGS := -fsanitize=address,undefined -fno-omit-frame-pointer

FUENTES := $(wildcard src/*.c)
PRUEBAS := tests/test_practica1.c
BINARIO := build/pruebas

.PHONY: all test sanitize clean

all: $(BINARIO)

$(BINARIO): $(FUENTES) $(PRUEBAS) | build
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $(FUENTES) $(PRUEBAS) -o $@

build:
	mkdir -p build

test: $(BINARIO)
	./$(BINARIO)

sanitize: clean | build
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $(SAN_FLAGS) $(FUENTES) $(PRUEBAS) -o $(BINARIO)
	ASAN_OPTIONS=detect_leaks=1 ./$(BINARIO)

clean:
	rm -rf build
