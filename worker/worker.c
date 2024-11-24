#include "calculation_service.h"
#include "discovery_service.h"

int main(int argc, char** argv) {
  pthread_t discovery_thread, calculation_thread;

  // Start the discovery service in a separate thread
  if (pthread_create(&discovery_thread, NULL, discovery_service, NULL) != 0) {
    perror("Failed to create discovery thread");
    exit(EXIT_FAILURE);
  }

  // Start the calculation service in a separate thread
  if (pthread_create(&calculation_thread, NULL, calculation_service, NULL) != 0) {
    perror("Failed to create calculation thread");
    exit(EXIT_FAILURE);
  }

  // Wait for threads to finish (they won't, as this is a long-running server)
  pthread_join(discovery_thread, NULL);
  pthread_join(calculation_thread, NULL);

  return 0;
}