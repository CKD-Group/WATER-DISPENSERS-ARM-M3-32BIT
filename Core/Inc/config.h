/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : config.h
  * @brief          : Configuration header for Water Dispenser Control System
  * @author         : Cuplis Kei Darma
  ******************************************************************************
  * This file contains all user-configurable parameters.
  * Modify this file to adapt the system to different hardware configurations.
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __CONFIG_H
#define __CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* ============================================================================
   HARDWARE CONFIGURATION - ACTIVE HIGH/LOW SELECTION
   ============================================================================
   Uncomment ONLY ONE option for each component to match your hardware
   ========================================================================== */

/* LED Configuration --------------------------------------------------------*/
// Program LED (Heartbeat) Polarity
#define PROGRAM_LED_ACTIVE_LOW   // Active Low (Common for built-in LEDs)
// #define PROGRAM_LED_ACTIVE_HIGH

// Status LED (Water/Fill) Polarity
// #define STATUS_LED_ACTIVE_LOW
#define STATUS_LED_ACTIVE_HIGH   // Active High

/* PUMP/RELAY Configuration -------------------------------------------------*/
// Choose Relay/Pump activation logic
//#define PUMP_ACTIVE_LOW         // Relay ON with GPIO_PIN_RESET (Most common 5V relay modules)
#define PUMP_ACTIVE_HIGH        // Relay ON with GPIO_PIN_SET

/* DOOR SWITCH Configuration ------------------------------------------------*/
// Choose door switch type and logic
//#define DOOR_SWITCH_TYPE_NO     // Normally Open switch (door closed = switch closes)
#define DOOR_SWITCH_TYPE_NC  // Normally Closed switch (door closed = switch opens)

#define DOOR_SWITCH_ACTIVE_LOW  // Switch reads LOW when triggered
// #define DOOR_SWITCH_ACTIVE_HIGH // Switch reads HIGH when triggered

/* WATER LIMIT SENSOR Configuration -----------------------------------------*/
// Choose water sensor type and logic
#define WATER_SENSOR_TYPE_NO    // Normally Open (tank full = switch closes)
//#define WATER_SENSOR_TYPE_NC     // Normally Closed (tank full = switch opens)

#define WATER_SENSOR_ACTIVE_LOW // Sensor reads LOW when tank is full
// #define WATER_SENSOR_ACTIVE_HIGH // Sensor reads HIGH when tank is full

/* OVERFLOW SENSOR Configuration (Optional) ---------------------------------*/
// Choose overflow sensor type and logic
#define OVERFLOW_SENSOR_TYPE_NO    // Normally Open (overflow = switch closes)
// #define OVERFLOW_SENSOR_TYPE_NC // Normally Closed (overflow = switch opens)

#define OVERFLOW_SENSOR_ACTIVE_LOW // Sensor reads LOW when overflow detected
// #define OVERFLOW_SENSOR_ACTIVE_HIGH // Sensor reads HIGH when overflow detected

/* ============================================================================
   TIMING CONFIGURATION
   ============================================================================
   Adjust these values based on your dispenser specifications
   All values in milliseconds (ms)
   ========================================================================== */

/* Pump Operation Timing ----------------------------------------------------*/
#define PUMP_NORMAL_FILL_TIME   360000  // Normal fill time: 6 minutes (360000 ms)
                                         // Adjust based on actual measurement
                                         // Modena DD 7106 L: typically 3-5 minutes

#define PUMP_MAX_RUN_TIME       540000  // Maximum pump run time: 9 minutes (540000 ms)
                                         // Safety timeout - pump will force stop
                                         // Should be ~1.5x normal fill time

#define MIN_PUMP_INTERVAL       10000   // Minimum time between pump cycles: 10 seconds
                                         // Prevents rapid cycling, protects pump
                                         // Recommended: 5-15 seconds

#define PUMP_STARTUP_DELAY      2000    // Delay after door closes: 2 seconds
                                         // Allows water to settle before pumping
                                         // Recommended: 1-3 seconds

/* Sensor Debounce Timing ---------------------------------------------------*/
#define DEBOUNCE_DELAY          100      // Debounce delay: 50 ms
                                         // Prevents false triggers from switch bounce
                                         // Recommended: 20-100 ms

/* LED Blink Timing ---------------------------------------------------------*/
#define LED_BLINK_FAST          250     // Fast blink rate: 250 ms (4 Hz)
                                         // Used for active states (filling, door open)

#define LED_BLINK_SLOW          500     // Slow blink rate: 500 ms (2 Hz)
                                         // Used for waiting states (cooldown, settle)

#define LED_BLINK_ERROR         125     // Error blink rate: 125 ms (8 Hz)
                                         // Used for error indication

/* ============================================================================
   SYSTEM PARAMETERS
   ============================================================================
   Tank and pump specifications for reference
   ========================================================================== */

/* Tank Specifications ------------------------------------------------------*/
#define ESTIMATED_TANK_SIZE     2000    // Tank capacity in ml (2000 ml = 2 liters)

#define TANK_TRIGGER_LEVEL      90      // Sensor trigger at 90% of capacity

/* Pump Specifications ------------------------------------------------------*/
#define ESTIMATED_PUMP_RATE     8       // Pump flow rate in ml/second
                                         // Calculate: TANK_SIZE / NORMAL_FILL_TIME

#define PUMP_POWER_RATING       10      // Pump power in Watts

/* ============================================================================
   SAFETY PARAMETERS
   ============================================================================
   Do not modify unless you know what you're doing
   ========================================================================== */

/* Pump Protection Parameters -----------------------------------------------*/
#define MAX_PUMP_DUTY_CYCLE     100     // Max 100% duty cycle (No forced cooldown)
#define DUTY_CYCLE_WINDOW       600000  // Over 10 minute window (ms)
#define PUMP_OVERHEAT_COOLDOWN  300000  // 5 min forced cooldown (ms)

/* Optional Features (Enable if hardware supported) -------------------------*/
#define ENABLE_BATTERY_MONITOR  0       // Requires ADC1
#define ENABLE_USAGE_STATS      0       // Requires Flash storage
#define ENABLE_REMOTE_MONITOR   0       // Requires UART1

/* Rapid Cycling Protection -------------------------------------------------*/
#define MAX_RAPID_CYCLES        10      // Maximum cycles before error check
#define MIN_AVG_CYCLE_TIME      30000   // Minimum average cycle time: 30 seconds
                                         // If average cycle < this, trigger error

/* Error Recovery -----------------------------------------------------------*/
#define ERROR_RESET_DOOR_TIME   3000    // Door must be open for 3 seconds to reset error

/* ============================================================================
   ERROR CODES
   ============================================================================ */
#define ERROR_NONE              0       // No error
#define ERROR_PUMP_TIMEOUT      1       // Pump ran too long (> MAX_RUN_TIME)
#define ERROR_SENSOR_FAULT      2       // Sensor appears to be faulty
#define ERROR_RAPID_CYCLING     3       // Too many rapid pump cycles detected
#define ERROR_GALLON_EMPTY      4       // Pump ran for normal fill time but tank not full (Gallon Empty)
#define ERROR_OVERFLOW          5       // Overflow sensor triggered

/* ============================================================================
   FEATURE ENABLE/DISABLE
   ============================================================================ */

// Enable/disable features
#define ENABLE_COOLDOWN_PERIOD  1       // 1 = Enable cooldown between cycles, 0 = Disable
#define ENABLE_STARTUP_DELAY    1       // 1 = Enable delay after door close, 0 = Disable
#define ENABLE_TIMEOUT_SAFETY   1       // 1 = Enable pump timeout protection, 0 = Disable
#define ENABLE_RAPID_CYCLE_CHECK 1      // 1 = Enable rapid cycling detection, 0 = Disable
#define ENABLE_OVERFLOW_SENSOR  0       // 1 = Enable overflow sensor, 0 = Disable (Default)

/* ============================================================================
   DERIVED MACROS - DO NOT MODIFY
   ============================================================================
   These macros are automatically generated based on above configuration
   ========================================================================== */

/* LED Control Macros -------------------------------------------------------*/
// Program LED
#ifdef PROGRAM_LED_ACTIVE_LOW
  #define PROGRAM_LED_ON()      HAL_GPIO_WritePin(LED_PROGRAM_RUNNING_GPIO_Port, LED_PROGRAM_RUNNING_Pin, GPIO_PIN_RESET)
  #define PROGRAM_LED_OFF()     HAL_GPIO_WritePin(LED_PROGRAM_RUNNING_GPIO_Port, LED_PROGRAM_RUNNING_Pin, GPIO_PIN_SET)
#else
  #define PROGRAM_LED_ON()      HAL_GPIO_WritePin(LED_PROGRAM_RUNNING_GPIO_Port, LED_PROGRAM_RUNNING_Pin, GPIO_PIN_SET)
  #define PROGRAM_LED_OFF()     HAL_GPIO_WritePin(LED_PROGRAM_RUNNING_GPIO_Port, LED_PROGRAM_RUNNING_Pin, GPIO_PIN_RESET)
#endif
#define PROGRAM_LED_TOGGLE()    HAL_GPIO_TogglePin(LED_PROGRAM_RUNNING_GPIO_Port, LED_PROGRAM_RUNNING_Pin)

// Status LED
#ifdef STATUS_LED_ACTIVE_LOW
  #define STATUS_LED_ON()       HAL_GPIO_WritePin(LED_STATUS_WATER_GALLON_GPIO_Port, LED_STATUS_WATER_GALLON_Pin, GPIO_PIN_RESET)
  #define STATUS_LED_OFF()      HAL_GPIO_WritePin(LED_STATUS_WATER_GALLON_GPIO_Port, LED_STATUS_WATER_GALLON_Pin, GPIO_PIN_SET)
#else
  #define STATUS_LED_ON()       HAL_GPIO_WritePin(LED_STATUS_WATER_GALLON_GPIO_Port, LED_STATUS_WATER_GALLON_Pin, GPIO_PIN_SET)
  #define STATUS_LED_OFF()      HAL_GPIO_WritePin(LED_STATUS_WATER_GALLON_GPIO_Port, LED_STATUS_WATER_GALLON_Pin, GPIO_PIN_RESET)
#endif
#define STATUS_LED_TOGGLE()     HAL_GPIO_TogglePin(LED_STATUS_WATER_GALLON_GPIO_Port, LED_STATUS_WATER_GALLON_Pin)

/* Pump Control Macros ------------------------------------------------------*/
#ifdef PUMP_ACTIVE_LOW
  #define PUMP_ON()   HAL_GPIO_WritePin(PUMP_WATER_GALLON_GPIO_Port, PUMP_WATER_GALLON_Pin, GPIO_PIN_RESET)
  #define PUMP_OFF()  HAL_GPIO_WritePin(PUMP_WATER_GALLON_GPIO_Port, PUMP_WATER_GALLON_Pin, GPIO_PIN_SET)
#else
  #define PUMP_ON()   HAL_GPIO_WritePin(PUMP_WATER_GALLON_GPIO_Port, PUMP_WATER_GALLON_Pin, GPIO_PIN_SET)
  #define PUMP_OFF()  HAL_GPIO_WritePin(PUMP_WATER_GALLON_GPIO_Port, PUMP_WATER_GALLON_Pin, GPIO_PIN_RESET)
#endif

/* Door Switch Reading Macros -----------------------------------------------*/
#if defined(DOOR_SWITCH_TYPE_NO) && defined(DOOR_SWITCH_ACTIVE_LOW)
  // NO switch, Active LOW: Door closed = switch closes = reads LOW
  #define IS_DOOR_CLOSED()  (HAL_GPIO_ReadPin(DOOR_SW_GPIO_Port, DOOR_SW_Pin) == GPIO_PIN_RESET)
  #define IS_DOOR_OPEN()    (HAL_GPIO_ReadPin(DOOR_SW_GPIO_Port, DOOR_SW_Pin) == GPIO_PIN_SET)

#elif defined(DOOR_SWITCH_TYPE_NO) && defined(DOOR_SWITCH_ACTIVE_HIGH)
  // NO switch, Active HIGH: Door closed = switch closes = reads HIGH
  #define IS_DOOR_CLOSED()  (HAL_GPIO_ReadPin(DOOR_SW_GPIO_Port, DOOR_SW_Pin) == GPIO_PIN_SET)
  #define IS_DOOR_OPEN()    (HAL_GPIO_ReadPin(DOOR_SW_GPIO_Port, DOOR_SW_Pin) == GPIO_PIN_RESET)

#elif defined(DOOR_SWITCH_TYPE_NC) && defined(DOOR_SWITCH_ACTIVE_LOW)
  // NC switch, Active LOW: Door closed = switch opens = reads HIGH (pulled up)
  #define IS_DOOR_CLOSED()  (HAL_GPIO_ReadPin(DOOR_SW_GPIO_Port, DOOR_SW_Pin) == GPIO_PIN_SET)
  #define IS_DOOR_OPEN()    (HAL_GPIO_ReadPin(DOOR_SW_GPIO_Port, DOOR_SW_Pin) == GPIO_PIN_RESET)

#elif defined(DOOR_SWITCH_TYPE_NC) && defined(DOOR_SWITCH_ACTIVE_HIGH)
  // NC switch, Active HIGH: Door closed = switch opens = reads LOW
  #define IS_DOOR_CLOSED()  (HAL_GPIO_ReadPin(DOOR_SW_GPIO_Port, DOOR_SW_Pin) == GPIO_PIN_RESET)
  #define IS_DOOR_OPEN()    (HAL_GPIO_ReadPin(DOOR_SW_GPIO_Port, DOOR_SW_Pin) == GPIO_PIN_SET)
#endif

/* Water Sensor Reading Macros ----------------------------------------------*/
#if defined(WATER_SENSOR_TYPE_NO) && defined(WATER_SENSOR_ACTIVE_LOW)
  // NO sensor, Active LOW: Tank full = sensor closes = reads LOW
  #define IS_TANK_FULL()    (HAL_GPIO_ReadPin(WATER_LIMIT_GPIO_Port, WATER_LIMIT_Pin) == GPIO_PIN_RESET)
  #define IS_TANK_EMPTY()   (HAL_GPIO_ReadPin(WATER_LIMIT_GPIO_Port, WATER_LIMIT_Pin) == GPIO_PIN_SET)

#elif defined(WATER_SENSOR_TYPE_NO) && defined(WATER_SENSOR_ACTIVE_HIGH)
  // NO sensor, Active HIGH: Tank full = sensor closes = reads HIGH
  #define IS_TANK_FULL()    (HAL_GPIO_ReadPin(WATER_LIMIT_GPIO_Port, WATER_LIMIT_Pin) == GPIO_PIN_SET)
  #define IS_TANK_EMPTY()   (HAL_GPIO_ReadPin(WATER_LIMIT_GPIO_Port, WATER_LIMIT_Pin) == GPIO_PIN_RESET)

#elif defined(WATER_SENSOR_TYPE_NC) && defined(WATER_SENSOR_ACTIVE_LOW)
  // NC sensor, Active LOW: Tank full = sensor opens = reads HIGH (pulled up)
  #define IS_TANK_FULL()    (HAL_GPIO_ReadPin(WATER_LIMIT_GPIO_Port, WATER_LIMIT_Pin) == GPIO_PIN_SET)
  #define IS_TANK_EMPTY()   (HAL_GPIO_ReadPin(WATER_LIMIT_GPIO_Port, WATER_LIMIT_Pin) == GPIO_PIN_RESET)

#elif defined(WATER_SENSOR_TYPE_NC) && defined(WATER_SENSOR_ACTIVE_HIGH)
  // NC sensor, Active HIGH: Tank full = sensor opens = reads LOW
  #define IS_TANK_FULL()    (HAL_GPIO_ReadPin(WATER_LIMIT_GPIO_Port, WATER_LIMIT_Pin) == GPIO_PIN_RESET)
  #define IS_TANK_EMPTY()   (HAL_GPIO_ReadPin(WATER_LIMIT_GPIO_Port, WATER_LIMIT_Pin) == GPIO_PIN_SET)
#endif

/* ============================================================================
   CONFIGURATION VALIDATION
   ============================================================================ */

// Compile-time checks for valid configuration
// Compile-time checks for valid configuration
#if !defined(PROGRAM_LED_ACTIVE_LOW) && !defined(PROGRAM_LED_ACTIVE_HIGH)
  #error "Program LED polarity not defined!"
#endif

#if !defined(STATUS_LED_ACTIVE_LOW) && !defined(STATUS_LED_ACTIVE_HIGH)
  #error "Status LED polarity not defined!"
#endif

#if !defined(PUMP_ACTIVE_LOW) && !defined(PUMP_ACTIVE_HIGH)
  #error "Pump polarity not defined! Define either PUMP_ACTIVE_LOW or PUMP_ACTIVE_HIGH"
#endif

#if !defined(DOOR_SWITCH_TYPE_NO) && !defined(DOOR_SWITCH_TYPE_NC)
  #error "Door switch type not defined! Define either DOOR_SWITCH_TYPE_NO or DOOR_SWITCH_TYPE_NC"
#endif

#if !defined(DOOR_SWITCH_ACTIVE_LOW) && !defined(DOOR_SWITCH_ACTIVE_HIGH)
  #error "Door switch active level not defined! Define either DOOR_SWITCH_ACTIVE_LOW or DOOR_SWITCH_ACTIVE_HIGH"
#endif

#if !defined(WATER_SENSOR_TYPE_NO) && !defined(WATER_SENSOR_TYPE_NC)
  #error "Water sensor type not defined! Define either WATER_SENSOR_TYPE_NO or WATER_SENSOR_TYPE_NC"
#endif

#if !defined(WATER_SENSOR_ACTIVE_LOW) && !defined(WATER_SENSOR_ACTIVE_HIGH)
  #error "Water sensor active level not defined! Define either WATER_SENSOR_ACTIVE_LOW or WATER_SENSOR_ACTIVE_HIGH"
#endif

#if ENABLE_OVERFLOW_SENSOR
  #if !defined(OVERFLOW_SENSOR_TYPE_NO) && !defined(OVERFLOW_SENSOR_TYPE_NC)
    #error "Overflow sensor type not defined!"
  #endif
  #if !defined(OVERFLOW_SENSOR_ACTIVE_LOW) && !defined(OVERFLOW_SENSOR_ACTIVE_HIGH)
    #error "Overflow sensor active level not defined!"
  #endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* __CONFIG_H */

/*******************************************************************************
 * CONFIGURATION GUIDE
 *******************************************************************************
 *
 * EXAMPLE CONFIGURATIONS:
 *
 * ============================================================================
 * CONFIGURATION 1: Standard Relay Module + NO Switches (MOST COMMON)
 * ============================================================================
 * #define LED_ACTIVE_LOW
 * #define PUMP_ACTIVE_LOW
 * #define DOOR_SWITCH_TYPE_NO
 * #define DOOR_SWITCH_ACTIVE_LOW
 * #define WATER_SENSOR_TYPE_NO
 * #define WATER_SENSOR_ACTIVE_LOW
 *
 * Wiring:
 * - Door Switch NO: One side to +3.3V (via 10k), other to PA0, PA0 to GND via switch
 * - Water Sensor NO: One side to +3.3V (via 10k), other to PA1, PA1 to GND via sensor
 * - Relay: Active LOW (most 5V relay modules)
 *
 * ============================================================================
 * CONFIGURATION 2: NC Switches for Safety
 * ============================================================================
 * #define LED_ACTIVE_LOW
 * #define PUMP_ACTIVE_LOW
 * #define DOOR_SWITCH_TYPE_NC
 * #define DOOR_SWITCH_ACTIVE_LOW
 * #define WATER_SENSOR_TYPE_NC
 * #define WATER_SENSOR_ACTIVE_LOW
 *
 * Wiring:
 * - Door Switch NC: Closed when door open, opens when door closes
 * - Water Sensor NC: Closed when tank empty, opens when tank full
 * - If wire breaks, system goes to safe state (pump won't run)
 *
 * ============================================================================
 * CONFIGURATION 3: Active HIGH system
 * ============================================================================
 * #define LED_ACTIVE_HIGH
 * #define PUMP_ACTIVE_HIGH
 * #define DOOR_SWITCH_TYPE_NO
 * #define DOOR_SWITCH_ACTIVE_HIGH
 * #define WATER_SENSOR_TYPE_NO
 * #define WATER_SENSOR_ACTIVE_HIGH
 *
 * Wiring:
 * - All signals are active HIGH
 * - Use for direct transistor drive circuits
 *
 ******************************************************************************/
