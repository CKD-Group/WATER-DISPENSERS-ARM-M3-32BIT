#ifndef ERROR_LOG_H
#define ERROR_LOG_H

#include "main.h"
#include "state_machine.h"

typedef struct {
  uint8_t errorCode;
  uint32_t timestamp;
  SystemState_t stateAtError;
  uint32_t pumpCycleCount;
} ErrorLog_t;

void ErrorLog_Add(uint8_t errorCode, SystemState_t state, uint32_t cycles);
ErrorLog_t* ErrorLog_Get(uint8_t index);
void ErrorLog_DisplayViaLED(void);

#endif // ERROR_LOG_H
