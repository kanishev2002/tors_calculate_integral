#include "send_to_worker.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#define CALCULATION_PORT 54321

int sendIntervalToWorker(Worker worker, TaskData task_data) {
  // Create a TCP socket
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("Failed to create socket");
    return -1;
  }

  // Set port to the TCP calculation service port from the discovery one.
  worker.address.sin_port = htons(CALCULATION_PORT);

  // Enable TCP keep-alive
  int keepalive = 1;   // Enable keep-alive
  int keepidle = 15;   // Time (in seconds) before sending keep-alive probes
  int keepintvl = 10;  // Interval (in seconds) between probes
  int keepcnt = 5;     // Number of unacknowledged probes before connection is closed

  if (setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive)) < 0) {
    perror("Failed to enable keep-alive");
    close(sockfd);
    return -1;
  }
#ifdef TCP_KEEPIDLE
  if (setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE, &keepidle, sizeof(keepidle)) < 0) {
    perror("Failed to set TCP_KEEPIDLE");
    close(sockfd);
    return -1;
  }
#endif
#ifdef TCP_KEEPINTVL
  if (setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL, &keepintvl, sizeof(keepintvl)) < 0) {
    perror("Failed to set TCP_KEEPINTVL");
    close(sockfd);
    return -1;
  }
#endif
#ifdef TCP_KEEPCNT
  if (setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT, &keepcnt, sizeof(keepcnt)) < 0) {
    perror("Failed to set TCP_KEEPCNT");
    close(sockfd);
    return -1;
  }
#endif

  // Connect to worker
  if (connect(sockfd, (struct sockaddr *)&worker.address, sizeof(worker.address)) < 0) {
    perror("Failed to connect to worker");
    close(sockfd);
    return -1;
  }

  // Send interval data to worker
  if (send(sockfd, &task_data, sizeof(task_data), 0) < 0) {
    perror("Failed to send interval to worker");
    close(sockfd);
    return -1;
  }

  // Return file descriptor
  return sockfd;
}
