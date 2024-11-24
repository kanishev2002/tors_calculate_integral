# Stage 1: Build the client
FROM debian:bullseye AS build

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential clang make && \
    rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source files and Makefile
COPY . /app/

# Build the client binary
RUN make clean
RUN make

# Stage 2: Runtime
FROM debian:bullseye-slim

# Copy the client binary
COPY --from=build /app/client /usr/local/bin/client

# Ensure the binary is executable
RUN chmod +x /usr/local/bin/client

# Set the default command
CMD ["client"]
