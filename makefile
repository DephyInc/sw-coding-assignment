CC = g++
ECHO = echo
MKDIR = mkdir -p
BUILD = build
SRC = src

# Compiler flags
CCFLAGS = -Wall -Wextra -I./include/
DEBUG_FLAGS = -g -DDEBUG
RELEASE_FLAGS = -O2

# Source files
SOURCES = $(shell find ./src/* -maxdepth 0 -type f \( -iname "*.cc" ! -iname "*_bk.cc" \) -printf '%f\n')

# Object files
DEBUG_OBJS = $(SOURCES:%.cc=build/debug/%.o)
RELEASE_OBJS = $(SOURCES:%.cc=build/release/%.o)

# Executable names
DEBUG_EXECUTABLE = eprom_debug
RELEASE_EXECUTABLE = eprom

# Default target (debug build)
all: $(DEBUG_EXECUTABLE) $(RELEASE_EXECUTABLE)

# Debug build
debug: $(RELEASE_EXECUTABLE)

$(DEBUG_EXECUTABLE): $(DEBUG_OBJS)
	$(CC) $(DEBUG_OBJS) -o $@

build/debug/%.o: src/%.cc
	@$(MKDIR) $(@D)
	$(CC) $(CCFLAGS) $(DEBUG_FLAGS) -c $< -o $@

# Release build
release: $(RELEASE_EXECUTABLE)

$(RELEASE_EXECUTABLE): $(RELEASE_OBJS)
	$(CC) $(RELEASE_OBJS) -o $@

build/release/%.o: src/%.cc
	@$(MKDIR) $(@D)
	$(CC) $(CCFLAGS) $(RELEASE_FLAGS) -c $< -o $@

# Clean up
clean:
	rm -r $(BUILD)

.PHONY: all release clean