#pragma once

#include "discover_workers.h"

typedef struct {
    int interval_id;
    double from;
    double to;
} TaskData;

/// Sends a task to calculate integral on [from; to] interval to the worker.
/// Returns a file descriptor with an open socket.
int sendIntervalToWorker(Worker worker, TaskData task_data);
