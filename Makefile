CC = gcc
MKDIR = mkdir -p
RM = rm -rf
CFLAGS = -Iinclude -Werror -Wall -Wextra -Wpedantic -g -DRAIZ_DEBUG
DEBUG_ARGS ?=

# Main target to build the executable
default build: clean
	$(MKDIR) ./build/bin
	$(CC) src/main.c -o ./build/bin/raiz $(CFLAGS)

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

debug: clean build
	clear && gdb --args ./build/bin/raiz $(DEBUG_ARGS)
