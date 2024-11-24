#include "discovery_service.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define DISCOVERY_PORT 12345
#define BUFFER_SIZE 256

int sockfd;

void cleanup_discovery(int sig) {
  printf("Discovery service stopping...\n");
  close(sockfd);
  exit(0);
}

void* discovery_service(void* args) {
  // Close the port when ctrl+c is pressed
  signal(SIGINT, cleanup_discovery);

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("Could not create UDP socket");
    exit(EXIT_FAILURE);
  }

  int reuseaddr = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr)) < 0) {
    perror("Could not set SO_REUSEADDR");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(DISCOVERY_PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    perror("Could not bind UDP socket");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  printf("Discovery service listening on UDP port %d...\n", DISCOVERY_PORT);

  char buffer[BUFFER_SIZE];
  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  char* response = "WORKER_AVAILABLE";

  while (1) {
    int bytes_received = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr*)&client_addr, &client_len);

    if (bytes_received > 0) {
      buffer[bytes_received] = '\0';
      printf("Received message: %s", buffer);
      if (strcmp(buffer, "DISCOVER") == 0) {
        printf("Discovery signal received. Responding...\n");
        sendto(sockfd, response, strlen(response), 0, (struct sockaddr*)&client_addr, client_len);
      }
    }
  }
  close(sockfd);
  return NULL;
}