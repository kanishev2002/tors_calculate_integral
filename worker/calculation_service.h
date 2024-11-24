#pragma once

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