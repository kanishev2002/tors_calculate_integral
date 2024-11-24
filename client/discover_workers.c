
#include "discover_workers.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BROADCAST_PORT 12345
#define BUFFER_SIZE 256

List discoverWorkers() {
  int sockfd;

  // Create UDP socket
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    perror("Failed to create socket");
    exit(EXIT_FAILURE);
  }
  puts("Created socket");

  // Enable broadcast
  int broadcast = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0) {
    perror("Failed to enable broadcast");
    close(sockfd);
    exit(EXIT_FAILURE);
  }
  puts("Enabled broadcast");

  // Set up broadcast address
  struct sockaddr_in broadcast_addr;
  memset(&broadcast_addr, 0, sizeof(broadcast_addr));
  broadcast_addr.sin_family = AF_INET;
  broadcast_addr.sin_port = htons(BROADCAST_PORT);
  broadcast_addr.sin_addr.s_addr = inet_addr("255.255.255.255");

  // Set socket receive timeout
  struct timeval timeout = {2, 0};  // 5 seconds timeout
  if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
    perror("Failed to set socket timeout");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  // Send broadcast message
  char *message = "DISCOVER";
  char buffer[BUFFER_SIZE];

  if (sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr)) < 0) {
    perror("Failed to send broadcast");
    close(sockfd);
    exit(EXIT_FAILURE);
  }
  puts("Sent broadcast message");

  // Receive response from workers
  struct sockaddr_in worker_addr;
  socklen_t addr_len = sizeof(worker_addr);

  List workers = {NULL, 0, 0};

  while (1) {
    int bytes_received = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&worker_addr, &addr_len);
    if (bytes_received < 0) {
      // Timeout or error occurred
      break;
    }

    printf("Worker found at IP: %s, Port: %d\n", inet_ntoa(worker_addr.sin_addr), ntohs(worker_addr.sin_port));

    Worker worker = {worker_addr};
    append_to_list(&workers, &worker, sizeof(worker));
  }

  close(sockfd);

  return workers;
}
