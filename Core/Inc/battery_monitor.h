#ifndef BATTERY_MONITOR_H
#define BATTERY_MONITOR_H

#include "main.h"

// Define this in config.h to enable
#ifndef ENABLE_BATTERY_MONITOR
#define ENABLE_BATTERY_MONITOR 0
#endif

void Battery_Check(void);
uint16_t Battery_GetVoltage_mV(void);

#endif // BATTERY_MONITOR_H
