/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : state_machine.h
  * @brief          : State machine header for Water Dispenser Control
  * @author         : Cuplis Kei Darma
  ******************************************************************************
  * This file contains state machine definitions and function prototypes
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __STATE_MACHINE_H
#define __STATE_MACHINE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "config.h"

/* Exported types ------------------------------------------------------------*/

/**
  * @brief  System State Enumeration
  */
typedef enum {
  STATE_IDLE = 0,       // System ready, tank OK
  STATE_DOOR_OPEN,      // Door is open, waiting for close
  STATE_WAIT_SETTLE,    // Waiting after door close (water settle)
  STATE_FILLING,        // Pump active, filling tank
  STATE_FULL,           // Tank is full, ready to use
  STATE_ERROR,          // Error condition detected
  STATE_COOLDOWN        // Pump cooldown period
} SystemState_t;

/**
  * @brief  System Statistics Structure
  */
typedef struct {
  uint32_t totalPumpRunTime;    // Total accumulated pump runtime (ms)
  uint32_t pumpCycleCount;      // Number of pump cycles since boot
  uint32_t lastFillDuration;    // Duration of last fill cycle (ms)
  uint32_t totalSystemUptime;   // Total time powered on
  uint32_t pumpAverageRuntime;  // Average per cycle
  uint32_t longestPumpRun;      // Longest single run
  uint32_t shortestPumpRun;     // Shortest single run
  uint8_t  pumpHealthScore;     // 0-100 health metric
  uint32_t errorCount;          // Total error count
  uint8_t  lastErrorCode;       // Last error code
} SystemStats_t;

/**
  * @brief  State Machine Context Structure
  */
typedef struct {
  SystemState_t currentState;   // Current system state
  SystemState_t previousState;  // Previous system state
  uint32_t stateChangeTime;     // Timestamp of last state change
  uint32_t pumpStartTime;       // Timestamp when pump started
  uint32_t pumpStopTime;        // Timestamp when pump stopped
  uint32_t lastBlinkTime;       // Timestamp for LED blinking
  uint8_t  ledBlinkState;       // LED blink state flag
  uint8_t  errorCode;           // Current error code
  SystemStats_t stats;          // System statistics
} StateMachine_t;

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/

/**
  * @brief  Initialize state machine
  * @param  None
  * @retval None
  */
void StateMachine_Init(void);

/**
  * @brief  Process state machine (call in main loop)
  * @param  None
  * @retval None
  */
void StateMachine_Process(void);

/**
  * @brief  Update LED indicators based on current state
  * @param  None
  * @retval None
  */
void StateMachine_UpdateLEDs(void);

/**
  * @brief  Get current system state
  * @param  None
  * @retval SystemState_t Current state
  */
SystemState_t StateMachine_GetState(void);

/**
  * @brief  Get current error code
  * @param  None
  * @retval uint8_t Error code (0 = no error)
  */
uint8_t StateMachine_GetErrorCode(void);

/**
  * @brief  Get system statistics
  * @param  None
  * @retval SystemStats_t* Pointer to statistics structure
  */
SystemStats_t* StateMachine_GetStats(void);

/**
  * @brief  Force reset error state
  * @param  None
  * @retval None
  */
void StateMachine_ResetError(void);

/**
  * @brief  Get state name string (for debugging)
  * @param  state State to get name for
  * @retval const char* State name string
  */
const char* StateMachine_GetStateName(SystemState_t state);

#ifdef __cplusplus
}
#endif

#endif /* __STATE_MACHINE_H */
