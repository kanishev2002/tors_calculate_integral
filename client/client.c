#include <arpa/inet.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "discover_workers.h"
#include "list.h"
#include "send_to_worker.h"

#define RESPONSE_TIMEOUT 5000  // Timeout in milliseconds per poll
#define BUFFER_SIZE 256

/// Awaits for all sockets to produce a response using poll mechanism.
/// Accepts an array of socket fds, number of fds, list of corresponding tasks and
/// an array of doubles that represents calculation results for each interval.
/// Returns a list of tasks that it was unable to complete due to network errors.
List wait_for_responses(int worker_sockets[], int num_workers, List* tasks, double* results) {
  struct pollfd fds[num_workers];
  char buffer[BUFFER_SIZE];
  int remaining_workers = num_workers;  // Track how many workers are left to respond
  int poll_count;
  TaskData* tasks_array = tasks->items;

  // Initialize pollfd structures for each worker socket
  for (int i = 0; i < num_workers; i++) {
    fds[i].fd = worker_sockets[i];
    fds[i].events = POLLIN;  // We are interested in reading responses
  }

  List remaining_tasks = {};

  // Loop until all workers respond or the timeout elapses
  while (remaining_workers > 0) {
    poll_count = poll(fds, num_workers, RESPONSE_TIMEOUT);

    if (poll_count < 0) {
      perror("poll error");
      break;
    } else if (poll_count == 0) {
      printf("Timeout waiting for workers. Remaining: %d\n", remaining_workers);
      break;
    }

    for (int i = 0; i < num_workers; i++) {
      if (fds[i].fd == -1) {
        continue;
      }
      TaskData task = tasks_array[i];
      if (fds[i].revents & POLLIN) {
        // Read response from the worker
        int bytes_read = recv(fds[i].fd, buffer, BUFFER_SIZE, 0);
        if (bytes_read > 0) {
          if (bytes_read == sizeof(double)) {
            double response = *(double*)buffer;
            printf("Response from worker %d: %f\n", i, response);
            results[task.interval_id] = response;
          } else {
            printf("Unexpected response size from worker %d.\n", i);
            append_to_list(&remaining_tasks, &task, sizeof(TaskData));
          }
        } else if (bytes_read == 0) {
          // Connection closed
          printf("Worker %d disconnected.\n", i);
          append_to_list(&remaining_tasks, &task, sizeof(TaskData));
        } else {
          perror("recv error");
          append_to_list(&remaining_tasks, &task, sizeof(TaskData));
        }
        // Mark this worker as handled
        close(fds[i].fd);
        fds[i].fd = -1;  // Ignore this socket in subsequent polls
        remaining_workers--;
      }
    }
  }

  if (remaining_workers > 0) {
    printf("Not all workers responded within the allotted time.\n");
  } else {
    printf("All workers responded.\n");
  }
  return remaining_tasks;
}

double calculate_kahan_sum(double* list, int size) {
  double sum = 0.0;
  double c = 0.0;
  for (int i = 0; i < size; ++i) {
    double value = list[i];
    double y = value - c;
    double t = sum + y;
    c = (t - sum) - y;
    sum = t;
  }
  return sum;
}

int main(int argc, char** argv) {
  printf("Client has started, looking for workers...\n");

  List workers;
  do {
    workers = discoverWorkers();
  } while (workers.length == 0);

  double from = 0, to = 100;
  unsigned workers_count = workers.length;
  printf("Found %d workers.\n", workers_count);
  double interval_length = to - from;

  List tasks = {};

  for (int i = 0; i < workers_count; ++i) {
    double from = (interval_length / workers_count) * i;
    double to = (interval_length / workers_count) * (i + 1);
    TaskData task_data = {i, from, to};
    append_to_list(&tasks, &task_data, sizeof(task_data));
  }

  printf("Created %d tasks.\n", tasks.length);

  unsigned tasks_cnt = tasks.length;
  double results[tasks_cnt];

  while (tasks.length != 0) {
    printf("Processing tasks...\n");
    int descriptors[workers.length];
    int num_tasks = 0;
    for (int i = 0; i < workers.length; ++i) {
      Worker worker = ((Worker*)workers.items)[i];
      TaskData task = ((TaskData*)tasks.items)[num_tasks];
      int fd = sendIntervalToWorker(worker, task);
      if (fd != -1) {
        descriptors[num_tasks++] = fd;
      }
      if (num_tasks == tasks.length) {
        break;
      }
    }
    // Await for tasks to complete
    List uncompleted_tasks = wait_for_responses(descriptors, num_tasks, &tasks, results);

    dispose_list(&workers);
    do {
      workers = discoverWorkers();
    } while (workers.length == 0);
    int remaining_tasks = tasks.length - num_tasks;
    // Move the unsubmitted tasks to the beginning
    memmove(tasks.items, tasks.items + num_tasks * sizeof(TaskData), remaining_tasks * sizeof(TaskData));
    if (uncompleted_tasks.length > 0) {
      // Append the tasks that could not complete
      memcpy(tasks.items + remaining_tasks * sizeof(TaskData), uncompleted_tasks.items,
             uncompleted_tasks.length * sizeof(TaskData));
    }
    tasks.length = remaining_tasks + uncompleted_tasks.length;
    printf("Finished processing some tasks. Tasks remaining: %d\n", tasks.length);
    dispose_list(&uncompleted_tasks);
  }
  double calc_result = calculate_kahan_sum(results, tasks_cnt);
  printf("Finished calculating. Result:\n");
  printf("Integral x*sin(x) from %f to %f = %f\n", from, to, calc_result);
  dispose_list(&tasks);
  dispose_list(&workers);
}