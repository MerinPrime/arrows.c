.PHONY: all clean native

CC=gcc
LDFLAGS=-L./lib -lm -lraylib -lopengl32 -lgdi32 -lwinmm
CFLAGS=-fopenmp -O3 -Wall -g -I./include

BUILD_DIR=build
TARGET_NAME=arrows
TARGET=$(BUILD_DIR)/$(TARGET_NAME)

SRC=$(wildcard src/*.c)
OBJ=$(subst src/,$(BUILD_DIR)/,$(SRC:.c=.o))

all: $(OBJ)
	$(CC) $(OBJ) $(CFLAGS) $(LDFLAGS) -o $(TARGET)

build/%.o: src/%.c
	-mkdir $(BUILD_DIR)
	$(CC) -c $(CFLAGS) -o $@ $^

native:
	"$(MAKE)" CFLAGS="$(CFLAGS) -march=native"

clean:
	rm -r $(BUILD_DIR)
