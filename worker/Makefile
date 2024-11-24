# worker/Makefile

# Compiler
CC = clang

# Source and Object Files
SRCS = worker.c calculation_service.c discovery_service.c
OBJS = $(SRCS:.c=.o)
BIN = worker

# Build type (default is debug)
BUILD_TYPE ?= debug

# Flags for different build types
CFLAGS_debug = -Wall -Wextra -g -fsanitize=address,undefined -fno-omit-frame-pointer
CFLAGS_release = -Wall -Wextra -O2 -DNDEBUG
CFLAGS_msan = -Wall -Wextra -g -fsanitize=memory -fsanitize-memory-track-origins -fno-omit-frame-pointer

LDFLAGS_debug = -fsanitize=address,undefined
LDFLAGS_release = -lpthread -lm
LDFLAGS_msan = -fsanitize=memory

# Select flags based on build type
CFLAGS = $(CFLAGS_$(BUILD_TYPE))
LDFLAGS = $(LDFLAGS_$(BUILD_TYPE))

# Default target
all: $(BIN)

# Link the binary
$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BIN) $(LDFLAGS)

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJS) $(BIN)

.PHONY: all clean
