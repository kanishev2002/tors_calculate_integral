#!/bin/zsh

# Number of workers to run
NUM_WORKERS=3

# Docker image names
WORKER_IMAGE="worker:release"
FAULTY_WORKER_IMAGE="worker:restarts"
CLIENT_IMAGE="client:release"

# Array to store worker container IDs
WORKER_CONTAINERS=()

# Function to clean up workers
cleanup_workers() {
    echo "Stopping all workers..."
    for container_id in "${WORKER_CONTAINERS[@]}"; do
        docker stop "$container_id" >/dev/null 2>&1
        docker rm "$container_id" >/dev/null 2>&1
    done
    echo "All workers stopped and removed."
}

# Trap to clean up workers on script exit
trap cleanup_workers EXIT

# Start workers in the background
echo "Starting $NUM_WORKERS worker containers..."
for i in $(seq 1 $NUM_WORKERS); do
    container_id=$(docker run -d --rm "$WORKER_IMAGE")
    WORKER_CONTAINERS+=("$container_id")
    echo "Worker $i started with container ID: $container_id"
done

# Start faulty worker in the background
echo "Starting faulty worker..."
container_id=$(docker run -d --rm "$FAULTY_WORKER_IMAGE")
WORKER_CONTAINERS+=("$container_id")
echo "Faulty worker started with container ID: $container_id"

# Run the client
echo "Starting the client..."
docker run --rm "$CLIENT_IMAGE"

# The client has exited, so the script will now clean up workers via the trap
echo "Client has exited. Cleaning up..."

