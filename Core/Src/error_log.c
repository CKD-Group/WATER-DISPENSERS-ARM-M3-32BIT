#include "error_log.h"
#include "config.h"

#define MAX_ERROR_LOG 10

static ErrorLog_t errorLog[MAX_ERROR_LOG];
static uint8_t errorLogIndex = 0;

/**
  * @brief  Log error to persistent storage (RAM buffer for now)
  */
void ErrorLog_Add(uint8_t errorCode, SystemState_t state, uint32_t cycles)
{
  errorLog[errorLogIndex].errorCode = errorCode;
  errorLog[errorLogIndex].timestamp = HAL_GetTick();
  errorLog[errorLogIndex].stateAtError = state;
  errorLog[errorLogIndex].pumpCycleCount = cycles;
  
  errorLogIndex = (errorLogIndex + 1) % MAX_ERROR_LOG;
  
  // TODO: Write to backup registers or flash if persistent storage is needed across resets
}

/**
  * @brief  Retrieve error log
  */
ErrorLog_t* ErrorLog_Get(uint8_t index)
{
  if(index >= MAX_ERROR_LOG) return NULL;
  return &errorLog[index];
}

/**
  * @brief  Display error log via LED pattern
  */
void ErrorLog_DisplayViaLED(void)
{
  // Blink error count
  // This is a blocking function for diagnostics
  for(int i = 0; i < errorLogIndex && i < 10; i++) {
    PROGRAM_LED_ON();
    HAL_Delay(100);
    PROGRAM_LED_OFF();
    HAL_Delay(200);
  }
}
