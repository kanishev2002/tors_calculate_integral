#include "calculation_service.h"

#include <signal.h>

int server_fd;

void cleanup_calculation(int sig) {
  printf("Calculation service stopping...\n");
  close(server_fd);
  exit(0);
}

double calculate_integral(double from, double to) { return (sin(to) - to * cos(to)) - (sin(from) - from * cos(from)); }

void* calculation_service(void* args) {
  // Close the port when ctrl+c is pressed
  signal(SIGINT, cleanup_calculation);

  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Could not create TCP socket");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(CALCULATION_PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    perror("Could not bind TCP socket");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  if (listen(server_fd, 5) < 0) {
    perror("Failed to listen on TCP socket");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  printf("Calculation service listening on TCP port %d...\n", CALCULATION_PORT);

  int client_fd;
  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);

  while (1) {
    if ((client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len)) < 0) {
      perror("Could not accept connection");
      continue;
    }

    printf("Connection established with client.\n");

    TaskData task;
    int bytes_received = recv(client_fd, &task, sizeof(task), 0);
    if (bytes_received == sizeof(task)) {
      printf("Received task: ID=%d, interval=[%.2f, %.2f]\n", task.interval_id, task.from, task.to);

      double result = calculate_integral(task.from, task.to);

      if (send(client_fd, &result, sizeof(result), 0) < 0) {
        perror("Could not send result");
      } else {
        printf("Sent result: %.2f\n", result);
      }
    } else {
      perror("Could not receive task data");
    }
    close(client_fd);
  }

  close(server_fd);
  return NULL;
}