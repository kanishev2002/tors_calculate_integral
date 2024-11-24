#pragma once

#include <netinet/in.h>
#include "list.h"

typedef struct {
  struct sockaddr_in address;
} Worker;

// typedef struct {
//   Worker* workers;
//   unsigned length;
//   unsigned capacity;
// } WorkersList;

// void append_to_workers_list(WorkersList* list, Worker worker);

/// Discovers workers on local network using UDP broadcast.
/// Returns a list of discovered workers.
List discoverWorkers();