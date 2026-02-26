CC = gcc
MKDIR = mkdir -p
RM = rm -rf
CFLAGS = -Iinclude -Werror -Wall -Wextra -Wpedantic -g
DEBUG_ARGS ?=

# Dynamically find all .c files
# (use find's -name for simplicity, no need for grep/|)
SOURCES := $(shell find ./src -type f -name '*.c')

# Convert sources to objects (assumes obj/ mirrors src/ structure,
# e.g., src/commands/foo.c -> obj/commands/foo.o)
OBJECTS := $(patsubst ./src/%.c, ./build/obj/%.o, $(SOURCES))

# Main target to build the executable
build/bin/raiz: $(OBJECTS)
	$(MKDIR) $(@D)
	$(CC) $^ -o $@ $(CFLAGS)

# Pattern rule to compile each .c to .o (creates subdirs as needed)
./build/obj/%.o: ./src/%.c clean
	$(MKDIR) $(@D)
	$(CC) -c $< -o $@ $(CFLAGS)

# Optional target to echo sources for debugging
list_sources:
	for FILE in $(SOURCES); do \
		echo $$FILE; \
	done

list_all: list_sources
	for FILE in $(shell find include -type f -name *.h); do \
		echo $$FILE; \
	done

clean:
	$(RM) build

debug: clean build/bin/raiz
	clear && gdb --args ./build/bin/raiz $(DEBUG_ARGS)
