#pragma once

#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>

#define DISCOVERY_PORT 12345
#define BUFFER_SIZE 256

/// Responds to UDP global broadcast on DISCOVERY_PORT to let the client know this worker exists.
void* discovery_service(void* args);
