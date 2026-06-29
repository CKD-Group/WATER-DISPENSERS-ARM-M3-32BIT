#ifndef REMOTE_MONITOR_H
#define REMOTE_MONITOR_H

#include "main.h"

// Define this in config.h to enable
#ifndef ENABLE_REMOTE_MONITOR
#define ENABLE_REMOTE_MONITOR 0
#endif

void Remote_Init(void);
void Remote_SendStatus(void);

#endif // REMOTE_MONITOR_H
