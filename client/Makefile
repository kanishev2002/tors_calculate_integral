# client/Makefile

CC = clang

# Common source files
SRCS = client.c discover_workers.c list.c send_to_worker.c
OBJS = $(SRCS:.c=.o)
BIN = client

# Build type (default to debug)
BUILD_TYPE ?= debug

# Flags for different build types
CFLAGS_debug = -Wall -Wextra -g -fsanitize=address,undefined
CFLAGS_release = -Wall -Wextra -O2 -DNDEBUG
LDFLAGS_debug = -fsanitize=address,undefined
LDFLAGS_release = 

# Select flags based on build type
CFLAGS = $(CFLAGS_$(BUILD_TYPE))
LDFLAGS = $(LDFLAGS_$(BUILD_TYPE))

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BIN) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(BIN)

.PHONY: all clean
