#pragma once

#include <math.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define CALCULATION_PORT 54321
#define BUFFER_SIZE 256

typedef struct {
  int interval_id;
  double from;
  double to;
} TaskData;

/// Calculates definite integral of function x*sin(x) on [from; to] interval
double calculate_integral(double from, double to);

/// Serves client on CALCULATION_PORT via a TCP connection.
/// Calculates integral on a given interval and returns it to the client.
void* calculation_service(void* args);