# Stage 1: Build the worker
FROM debian:bullseye AS build

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential clang make && \
    rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source files and Makefile
COPY . /app/

# Allow dynamic build type selection via build arguments
ARG BUILD_TYPE=debug

# Build the worker binary
RUN make clean
RUN make BUILD_TYPE=${BUILD_TYPE}

# Stage 2: Runtime
FROM debian:bullseye-slim

# Copy the worker binary
COPY --from=build /app/worker /usr/local/bin/worker

# Ensure the binary is executable
RUN chmod +x /usr/local/bin/worker

# Set the default command
CMD ["worker"]
