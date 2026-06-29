#include "remote_monitor.h"
#include "config.h"
#include "state_machine.h"
#include "battery_monitor.h"
#include <stdio.h>
#include <string.h>

#if ENABLE_REMOTE_MONITOR

// Assumes UART1 is initialized and handle is huart1
extern UART_HandleTypeDef huart1;

/**
  * @brief  Initialize remote monitoring
  */
void Remote_Init(void)
{
  // Any specific init if needed
}

/**
  * @brief  Send system status via UART
  */
void Remote_SendStatus(void)
{
  char buffer[128];
  SystemState_t state = StateMachine_GetState();
  SystemStats_t* stats = StateMachine_GetStats();
  
  // Format JSON-like string
  // {"state":"IDLE","err":0,"cycles":123,"bat":3300}
  sprintf(buffer, "{\"state\":\"%s\",\"err\":%d,\"cycles\":%lu,\"bat\":%d}\r\n",
          StateMachine_GetStateName(state),
          stats->lastErrorCode,
          stats->pumpCycleCount,
          Battery_GetVoltage_mV());
          
  HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 100);
}

#else

// Stubs
void Remote_Init(void) {}
void Remote_SendStatus(void) {}

#endif // ENABLE_REMOTE_MONITOR
