#pragma once

/// Responds to UDP global broadcast on DISCOVERY_PORT to let the client know this worker exists.
void* discovery_service(void* args);
