# Water Dispenser Control System Documentation

## Overview
This project implements a robust control system for a water dispenser using an STM32F103C8T6 ("Blue Pill") microcontroller. It manages the water pump, monitors sensors (door switch, water level), and provides status feedback via LEDs.

**Version**: 2.1.0
**Focus**: Efficiency, Safety, and Reliability.

## Directory Structure (`Core`)
The core logic is contained within the `Core` directory:
- **`Inc/`**: Header files.
- **`Src/`**: Source files.
- **`Startup/`**: Assembly startup code.

### Key Files
| File | Description |
|------|-------------|
| `config.h` | **Primary Configuration File**. Contains all user-adjustable parameters (timings, polarity, sensor types). |
| `state_machine.c/.h` | Implements the core system logic using a finite state machine. |
| `sensors.c/.h` | Handles sensor readings with debouncing and abstraction. |
| `main.c` | Entry point, hardware initialization, and main loop. |
| `error_log.c/.h` | **[NEW]** Persistent error logging module. |
| `config_storage.c/.h` | **[NEW]** Flash configuration storage module. |

## System Architecture

### 1. State Machine (`state_machine.c`)
The system operates based on a Finite State Machine (FSM) with the following states:

| State | Description |
|-------|-------------|
| `STATE_IDLE` | System is ready. Waiting for door open or tank empty signal. |
| `STATE_DOOR_OPEN` | Door is open. Pump is forced OFF. Waiting for door to close. |
| `STATE_WAIT_SETTLE` | Waiting period after door closes to allow water level to stabilize. |
| `STATE_FILLING` | Pump is ON. Filling the tank until the sensor triggers or timeout occurs. |
| `STATE_FULL` | Tank is full. Pump is OFF. |
| `STATE_COOLDOWN` | **[NEW]** Cooling down period if pump duty cycle limit is exceeded. |
| `STATE_ERROR` | Error condition (e.g., pump timeout, sensor fault). LEDs blink rapidly. |

### 2. Sensor Module (`sensors.c`)
Handles inputs from the physical hardware:
- **Door Switch**: Detects if the dispenser door is open.
- **Water Level Sensor**: Detects if the tank is full.
- **Debouncing**: Implements software debouncing (default 50ms) to prevent false triggers.
- **Self-Test**: **[NEW]** Runs a sensor health check at startup.

### 3. Configuration (`config.h`)
The system is highly configurable. Key settings include:
- **Hardware Polarity**: Independent Active LOW/HIGH support for Program LED, Status LED, Pump, Sensors, and Overflow Sensor.
- **Sensor Types**: Support for Normally Open (NO) or Normally Closed (NC) switches.
- **Timings**:
  - `PUMP_NORMAL_FILL_TIME`: Expected fill time (default 6 min).
  - `PUMP_MAX_RUN_TIME`: Safety timeout (default 9 min).
  - `MIN_PUMP_INTERVAL`: Pump protection delay.

## New Features (v2.1.0)

### 1. Efficiency & Motor Protection ⚡
- **Non-Blocking Main Loop**: Adaptive loop rate (10ms active / 50ms idle) reduces CPU usage.
- **Duty Cycle Limiter**: Limits pump usage to **30%** over a 10-minute window to prevent overheating.
- **Forced Cooldown**: Enforces a 5-minute cooldown if the duty cycle limit is exceeded.

### 2. Safety & Reliability 🛡️
- **Brown-Out Detection**: Detects power dips and blinks LEDs 5 times on startup.
- **State Validation**: Prevents illegal state transitions (e.g., Error -> Filling without reset).
- **Graceful Shutdown**: Implemented `System_Shutdown()` for safe power-down.
- **IWDG Watchdog**: Refreshes every 3 seconds to prevent system hangs.

### 3. Advanced Diagnostics 📊
- **Persistent Error Logging**: Stores the last 10 errors in a circular buffer.
- **Diagnostic Mode**: Hold door open for **10 seconds** to trigger. Displays:
  - Clock speed check
  - Sensor status
  - Error counts
  - Pump cycle counts
- **Config Storage**: Framework for saving settings to Flash memory.

### 4. Optional Features 💡
(Disabled by default in `config.h`)
- **Battery Monitor**: Checks voltage via ADC.
- **Usage Statistics**: Tracks total liters pumped.
- **Remote Monitor**: Sends status JSON via UART.

## LED Patterns Report
The system uses two LEDs to communicate status:
- **Program LED** (System Heartbeat)
- **Status LED** (Water/Fill Status)

| System State | Program LED | Status LED | Meaning |
|--------------|-------------|------------|---------|
| **IDLE** | **ON** (Solid) | **OFF** | System ready, waiting for trigger. |
| **DOOR OPEN** | **Fast Blink** (250ms) | **OFF** | Door is open. Close door to proceed. |
| **WAIT SETTLE** | **ON** (Solid) | **Slow Blink** (500ms) | Waiting for water to settle after door close. |
| **FILLING** | **ON** (Solid) | **Fast Blink** (250ms) | Pump is active, filling tank. |
| **FULL** | **ON** (Solid) | **ON** (Solid) | Tank is full and ready. |
| **COOLDOWN** | **ON** (Solid) | **Slow Blink** (500ms) | Pump cooling down (overheat protection). |
| **ERROR** | **Fast Blink** | **Fast Blink** | **Critical Error**. LEDs blink alternatingly. |

## Hardware Pinout
Defined in `main.h`:
- **Pump Control**: `GPIOC Pin 15`
- **Program LED**: `GPIOC Pin 13` (System Heartbeat)
- **Status LED**: `GPIOC Pin 14` (Water/Fill Status)
- **Door Switch**: `GPIOA Pin 0`
- **Water Sensor**: `GPIOA Pin 1`

## Verification Checklist
Since this is an embedded system, verification requires manual testing on the hardware.

1. **Boot Test**: Verify system boots normally (3x LED blink).
2. **Noise Test**: Rapidly toggle the door switch. The system should NOT cycle states rapidly (due to debounce).
3. **Stress Test**: Trigger the water sensor repeatedly. After ~3 minutes of cumulative runtime (within 10 mins), the system should enter ERROR/COOLDOWN state.
4. **Stability Test**: Leave the system running for 1 hour. It should not reset.
5. **Diagnostic Test**: Hold door open for 10 seconds. Verify LEDs start blinking diagnostic patterns.
6. **Brown-Out Test**: Briefly disconnect power. Verify system blinks 5 times on restart.

## Error Codes
| Code | Meaning |
|------|---------|
| `0` | No Error |
| `1` | **Pump Timeout**: Pump ran longer than `PUMP_MAX_RUN_TIME`. |
| `2` | **Sensor Fault**: Unexpected sensor behavior. |
| `3` | **Rapid Cycling**: Pump is cycling too frequently. |
| `4` | **Gallon Empty**: Pump ran for normal fill time but tank is not full. |
| `5` | **Overflow**: Optional overflow sensor triggered. |
