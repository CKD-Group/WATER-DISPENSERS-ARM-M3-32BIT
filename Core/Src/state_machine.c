/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : state_machine.c
  * @brief          : State machine implementation for Water Dispenser Control
  * @author         : Cuplis Kei Darma
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "state_machine.h"
#include "sensors.h"
#include "error_log.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static StateMachine_t sm;  // State machine context
static uint32_t pumpOnTimeWindow = 0;
static uint32_t windowStartTime = 0;

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Initialize state machine
  * @param  None
  * @retval None
  */
void StateMachine_Init(void)
{
  sm.currentState = STATE_IDLE;
  sm.previousState = STATE_IDLE;
  sm.stateChangeTime = 0;
  sm.pumpStartTime = 0;
  sm.errorCode = ERROR_NONE;
  sm.stats.pumpCycleCount = 0;
  sm.stats.totalPumpRunTime = 0;
  sm.stats.errorCount = 0;
  
  // Initial LED state
  StateMachine_UpdateLEDs();
}

/**
  * @brief  Get current system state
  * @param  None
  * @retval SystemState_t Current state
  */
SystemState_t StateMachine_GetState(void)
{
  return sm.currentState;
}

/**
  * @brief  Get current error code
  * @param  None
  * @retval uint8_t Error code
  */
uint8_t StateMachine_GetErrorCode(void)
{
  return sm.errorCode;
}

/**
  * @brief  Update LED indicators based on current state
  * @param  None
  * @retval None
  */
void StateMachine_UpdateLEDs(void)
{
  uint32_t currentTime = HAL_GetTick();
  
  // Handle blinking logic
  if((currentTime - sm.lastBlinkTime) >= 250) { // Default fast blink base
    sm.lastBlinkTime = currentTime;
    sm.ledBlinkState = !sm.ledBlinkState;
  }
  
  switch(sm.currentState) {
    case STATE_IDLE:
      PROGRAM_LED_ON();
      STATUS_LED_OFF();
      break;
      
    case STATE_DOOR_OPEN:
      // Fast blink Program LED
      if(sm.ledBlinkState) PROGRAM_LED_ON(); else PROGRAM_LED_OFF();
      STATUS_LED_OFF();
      break;
      
    case STATE_WAIT_SETTLE:
      PROGRAM_LED_ON();
      // Slow blink Status LED (500ms)
      if((currentTime / 500) % 2) STATUS_LED_ON(); else STATUS_LED_OFF();
      break;
      
    case STATE_FILLING:
      PROGRAM_LED_ON();
      // Fast blink Status LED
      if(sm.ledBlinkState) STATUS_LED_ON(); else STATUS_LED_OFF();
      break;
      
    case STATE_FULL:
      PROGRAM_LED_ON();
      STATUS_LED_ON();
      break;
      
    case STATE_ERROR:
      // Alternating blink
      if(sm.ledBlinkState) {
        PROGRAM_LED_ON();
        STATUS_LED_OFF();
      } else {
        PROGRAM_LED_OFF();
        STATUS_LED_ON();
      }
      break;
      
    case STATE_COOLDOWN:
      PROGRAM_LED_ON();
      // Slow blink Status LED
      if((currentTime / 500) % 2) STATUS_LED_ON(); else STATUS_LED_OFF();
      break;
  }
}

/* Private function prototypes -----------------------------------------------*/
static void EnterState(SystemState_t newState);
static uint8_t CheckSafetyConditions(void);
static uint8_t CheckPumpDutyCycle(void);
static void UpdatePumpStatistics(uint32_t runtime);

// Handlers
static void HandleIdleState(void);
static void HandleDoorOpenState(void);
static void HandleWaitSettleState(void);
static void HandleFillingState(void);
static void HandleFullState(void);
static void HandleErrorState(void);
static void HandleCooldownState(void);

// ... (Rest of the file content, ensuring unused functions are removed or used)

// NOTE: The previous error showed these were defined but not used.
// This means they are NOT called in StateMachine_Process.
// I need to ensure StateMachine_Process actually calls them.

/**
  * @brief  Process state machine (call in main loop)
  * @param  None
  * @retval None
  */
void StateMachine_Process(void)
{
  // Global safety check removed to prevent blocking state handlers.
  // Safety is handled by individual state handlers and the Critical Safety Override below.

  // CRITICAL SAFETY OVERRIDE
  // Priority 1: Prevent Overflow
  // If tank is full, FORCE PUMP OFF immediately, regardless of state.
  if(Sensors_IsTankFull()) {
     PUMP_OFF();
     
     // If we were filling, transition state to record it properly
     if(sm.currentState == STATE_FILLING) {
         EnterState(STATE_FULL);
     }
  }

  switch (sm.currentState)
  {
    case STATE_IDLE:
      HandleIdleState();
      break;
    case STATE_DOOR_OPEN:
      HandleDoorOpenState();
      break;
    case STATE_WAIT_SETTLE:
      HandleWaitSettleState();
      break;
    case STATE_FILLING:
      HandleFillingState();
      break;
    case STATE_FULL:
      HandleFullState();
      break;
    case STATE_ERROR:
      HandleErrorState();
      break;
    case STATE_COOLDOWN:
      HandleCooldownState();
      break;
    default:
      // Should not happen, reset to IDLE
      EnterState(STATE_IDLE);
      break;
  }
}


/**
  * @brief  Get system statistics
  * @param  None
  * @retval SystemStats_t* Pointer to statistics structure
  */
SystemStats_t* StateMachine_GetStats(void)
{
  return &sm.stats;
}

/**
  * @brief  Force reset error state
  * @param  None
  * @retval None
  */
void StateMachine_ResetError(void)
{
  sm.errorCode = ERROR_NONE;
  sm.stats.pumpCycleCount = 0;
  sm.stats.totalPumpRunTime = 0;
  EnterState(STATE_IDLE);
}

/**
  * @brief  Get state name string (for debugging)
  * @param  state State to get name for
  * @retval const char* State name string
  */
const char* StateMachine_GetStateName(SystemState_t state)
{
  switch(state)
  {
    case STATE_IDLE:        return "IDLE";
    case STATE_DOOR_OPEN:   return "DOOR_OPEN";
    case STATE_WAIT_SETTLE: return "WAIT_SETTLE";
    case STATE_FILLING:     return "FILLING";
    case STATE_FULL:        return "FULL";
    case STATE_ERROR:       return "ERROR";
    case STATE_COOLDOWN:    return "COOLDOWN";
    default:                return "UNKNOWN";
  }
}

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Enter new state with timestamp
  * @param  newState Target state
  * @retval None
  */
static void EnterState(SystemState_t newState)
{
  sm.previousState = sm.currentState;
  sm.currentState = newState;
  sm.stateChangeTime = HAL_GetTick();
  sm.ledBlinkState = 0;
  sm.lastBlinkTime = sm.stateChangeTime;
  
  // Log error if entering error state (Task 7)
  if(newState == STATE_ERROR) {
    ErrorLog_Add(sm.errorCode, sm.previousState, sm.stats.pumpCycleCount);
  }
}

/**
  * @brief  Check safety conditions before pump operation
  * @retval 1 if safe, 0 if unsafe
  */
static uint8_t CheckSafetyConditions(void)
{
  uint32_t currentTime = HAL_GetTick();

  // Check if door is closed
  if(!Sensors_IsDoorClosed()) {
    return 0;
  }

  // Check minimum interval between pump cycles
  #if ENABLE_COOLDOWN_PERIOD
  if(sm.pumpStopTime > 0) {
    if((currentTime - sm.pumpStopTime) < MIN_PUMP_INTERVAL) {
      return 0;
    }
  }
  #endif

  // Check for rapid cycling (potential sensor fault)
  #if ENABLE_RAPID_CYCLE_CHECK
  if(sm.stats.pumpCycleCount > MAX_RAPID_CYCLES) {
    uint32_t avgCycleTime = sm.stats.totalPumpRunTime / sm.stats.pumpCycleCount;
    if(avgCycleTime < MIN_AVG_CYCLE_TIME) {
      sm.errorCode = ERROR_RAPID_CYCLING;
      return 0;
    }
  }
  #endif

  return 1;
}

/**
  * @brief  Handle IDLE state
  * @retval None
  */
static void HandleIdleState(void)
{
  PUMP_OFF();

  // Check if door opened
  if(!Sensors_IsDoorClosed()) {
    EnterState(STATE_DOOR_OPEN);
    return;
  }

  // Check if tank needs refilling
  if(Sensors_IsTankEmpty()) {
    if(CheckSafetyConditions()) {
      #if ENABLE_STARTUP_DELAY
      EnterState(STATE_WAIT_SETTLE);
      #else
      EnterState(STATE_FILLING);
      PUMP_ON();
      sm.pumpStartTime = HAL_GetTick();
      sm.stats.pumpCycleCount++;
      #endif
    }
  } else if(Sensors_IsTankFull()) {
    EnterState(STATE_FULL);
  }
}

/**
  * @brief  Handle DOOR_OPEN state
  * @retval None
  */
static void HandleDoorOpenState(void)
{
  PUMP_OFF();

  // Wait for door to close
  if(Sensors_IsDoorClosed()) {
    #if ENABLE_STARTUP_DELAY
    EnterState(STATE_WAIT_SETTLE);
    #else
    if(Sensors_IsTankFull()) {
      EnterState(STATE_FULL);
    } else {
      EnterState(STATE_IDLE);
    }
    #endif
  }
}

/**
  * @brief  Handle WAIT_SETTLE state
  * @retval None
  */
static void HandleWaitSettleState(void)
{
  uint32_t currentTime = HAL_GetTick();
  uint32_t timeInState = currentTime - sm.stateChangeTime;

  PUMP_OFF();

  // Door opened again
  if(!Sensors_IsDoorClosed()) {
    EnterState(STATE_DOOR_OPEN);
    return;
  }

  // Wait for settling time
  if(timeInState >= PUMP_STARTUP_DELAY) {
    if(Sensors_IsTankFull()) {
      EnterState(STATE_FULL);
    } else if(Sensors_IsTankEmpty()) {
      if(CheckSafetyConditions()) {
        EnterState(STATE_FILLING);
        PUMP_ON();
        sm.pumpStartTime = currentTime;
        sm.stats.pumpCycleCount++;
      } else {
        EnterState(STATE_ERROR);
      }
    } else {
      EnterState(STATE_IDLE);
    }
  }
}

/**
  * @brief  Handle FILLING state
  * @retval None
  */
static void HandleFillingState(void)
{
  uint32_t currentTime = HAL_GetTick();
  uint32_t pumpRunTime = currentTime - sm.pumpStartTime;

  // Check duty cycle
  if(!CheckPumpDutyCycle()) {
    PUMP_OFF();
    sm.pumpStopTime = currentTime;
    UpdatePumpStatistics(pumpRunTime);
    sm.stats.errorCount++;
    // Force cooldown
    EnterState(STATE_COOLDOWN); 
    return;
  }

  // Safety check: overflow detected
  if(Sensors_IsOverflow()) {
    PUMP_OFF();
    sm.pumpStopTime = currentTime;
    sm.stats.totalPumpRunTime += pumpRunTime;
    sm.stats.lastFillDuration = pumpRunTime;
    sm.errorCode = ERROR_OVERFLOW;
    sm.stats.errorCount++;
    sm.stats.lastErrorCode = ERROR_OVERFLOW;
    EnterState(STATE_ERROR);
    return;
  }

  // Safety check: door opened during filling
  if(!Sensors_IsDoorClosed()) {
    PUMP_OFF();
    sm.pumpStopTime = currentTime;
    sm.stats.totalPumpRunTime += pumpRunTime;
    sm.stats.lastFillDuration = pumpRunTime;
    EnterState(STATE_DOOR_OPEN);
    return;
  }

  // Tank full - normal completion
  if(Sensors_IsTankFull()) {
    PUMP_OFF();
    sm.pumpStopTime = currentTime;
    UpdatePumpStatistics(pumpRunTime);
    EnterState(STATE_FULL);
    return;
  }

  #if ENABLE_TIMEOUT_SAFETY
  // Safety timeout - maximum run time exceeded
  if(pumpRunTime > PUMP_MAX_RUN_TIME) {
    PUMP_OFF();
    sm.pumpStopTime = currentTime;
    sm.stats.totalPumpRunTime += pumpRunTime;
    sm.stats.lastFillDuration = pumpRunTime;
    sm.errorCode = ERROR_PUMP_TIMEOUT;
    sm.stats.errorCount++;
    sm.stats.lastErrorCode = ERROR_PUMP_TIMEOUT;
    EnterState(STATE_ERROR);
    return;
  }

  // Auto-stop at normal fill time if sensor doesn't trigger
  // (backup safety in case sensor fails high)
  if(pumpRunTime > PUMP_NORMAL_FILL_TIME && !Sensors_IsTankFull()) {
    PUMP_OFF();
    sm.pumpStopTime = currentTime;
    sm.stats.totalPumpRunTime += pumpRunTime;
    sm.stats.lastFillDuration = pumpRunTime;
    sm.errorCode = ERROR_GALLON_EMPTY;
    sm.stats.errorCount++;
    sm.stats.lastErrorCode = ERROR_GALLON_EMPTY;
    EnterState(STATE_ERROR);
    return;
  }
  #endif
}

/**
  * @brief  Handle FULL state
  * @retval None
  */
static void HandleFullState(void)
{
  PUMP_OFF();

  // Door opened
  if(!Sensors_IsDoorClosed()) {
    EnterState(STATE_DOOR_OPEN);
    return;
  }

  // Tank no longer full (water consumed)
  if(Sensors_IsTankEmpty()) {
    #if ENABLE_COOLDOWN_PERIOD
    EnterState(STATE_COOLDOWN);
    #else
    EnterState(STATE_WAIT_SETTLE);
    #endif
  }
}

/**
  * @brief  Handle ERROR state
  * @retval None
  */
static void HandleErrorState(void)
{
  uint32_t currentTime = HAL_GetTick();
  uint32_t timeInState = currentTime - sm.stateChangeTime;

  PUMP_OFF();

  // Error can be cleared by opening door for specified time
  if(!Sensors_IsDoorClosed()) {
    if(timeInState > ERROR_RESET_DOOR_TIME) {
      StateMachine_ResetError();
      EnterState(STATE_DOOR_OPEN);
    }
  } else {
    // Reset timer when door closes
    sm.stateChangeTime = currentTime;
  }
}

/**
  * @brief  Handle COOLDOWN state
  * @retval None
  */
static void HandleCooldownState(void)
{
  uint32_t currentTime = HAL_GetTick();
  uint32_t timeInState = currentTime - sm.stateChangeTime;

  PUMP_OFF();

  // Door opened
  if(!Sensors_IsDoorClosed()) {
    EnterState(STATE_DOOR_OPEN);
    return;
  }

  // Wait minimum interval before allowing refill
  if(timeInState >= MIN_PUMP_INTERVAL) {
    if(Sensors_IsTankEmpty()) {
      EnterState(STATE_WAIT_SETTLE);
    } else if(Sensors_IsTankFull()) {
      EnterState(STATE_FULL);
    } else {
      EnterState(STATE_IDLE);
    }
  }
}

/**
  * @brief  Check pump duty cycle
  * @retval 1 if OK, 0 if over limit
  */
static uint8_t CheckPumpDutyCycle(void)
{
  uint32_t currentTime = HAL_GetTick();
  static uint32_t lastCheck = 0;
  
  if (lastCheck == 0) lastCheck = currentTime;
  
  // Fix: If lastCheck is older than current pump start, we missed the gap.
  // Resync to pumpStartTime to avoid counting OFF time as ON time.
  if (lastCheck < sm.pumpStartTime) {
    lastCheck = sm.pumpStartTime;
  }

  uint32_t delta = currentTime - lastCheck;
  lastCheck = currentTime;
  
  // Start new window if needed
  if(windowStartTime == 0 || (currentTime - windowStartTime) > DUTY_CYCLE_WINDOW) {
    windowStartTime = currentTime;
    pumpOnTimeWindow = 0;
  }
  
  pumpOnTimeWindow += delta;
  
  uint32_t windowDuration = currentTime - windowStartTime;
  if(windowDuration < 1000) return 1; // Too short to judge
  
  uint32_t dutyCycle = (pumpOnTimeWindow * 100) / windowDuration;
  
  if(dutyCycle > MAX_PUMP_DUTY_CYCLE) {
    return 0;
  }
  
  return 1;
}

/**
  * @brief  Calculate pump health score
  * @retval uint8_t 0-100 (100 = perfect health)
  */
static uint8_t CalculatePumpHealth(void)
{
  uint8_t score = 100;
  
  // Deduct for errors
  if(sm.stats.errorCount > 10) score -= 20;
  else if(sm.stats.errorCount > 5) score -= 10;
  
  // Deduct for excessive runtime
  if(sm.stats.pumpAverageRuntime > (PUMP_NORMAL_FILL_TIME * 1.5)) {
    score -= 15;
  }
  
  // Deduct for rapid cycling
  if(sm.stats.pumpCycleCount > 1000) {
    score -= 10;
  }
  
  return score;
}

/**
  * @brief  Update statistics after pump stop
  */
static void UpdatePumpStatistics(uint32_t runtime)
{
  sm.stats.totalPumpRunTime += runtime;
  sm.stats.lastFillDuration = runtime;
  
  // Update longest/shortest
  if(runtime > sm.stats.longestPumpRun) {
    sm.stats.longestPumpRun = runtime;
  }
  if(sm.stats.shortestPumpRun == 0 || 
     runtime < sm.stats.shortestPumpRun) {
    sm.stats.shortestPumpRun = runtime;
  }
  
  // Calculate average
  if(sm.stats.pumpCycleCount > 0) {
    sm.stats.pumpAverageRuntime = 
      sm.stats.totalPumpRunTime / sm.stats.pumpCycleCount;
  }
  
  // Update health score
  sm.stats.pumpHealthScore = CalculatePumpHealth();
}
