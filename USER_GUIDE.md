# Water Dispenser Control System - User Guide

## Hardware Pinout

### STM32F103C8T6 Pin Assignments

| Pin | Label | Type | Description |
|-----|-------|------|-------------|
| **PC13** | `LED_PROGRAM_RUNNING` | Output | Program LED (System Heartbeat) - Active LOW |
| **PC14** | `LED_STATUS_WATER_GALLON` | Output | Status LED (Water/Fill Status) - Active HIGH |
| **PC15** | `PUMP_WATER_GALLON` | Output | Pump/Relay Control - Active HIGH |
| **PA0** | `DOOR_SW` | Input | Door Switch - NO, Active LOW |
| **PA1** | `WATER_LIMIT` | Input | Water Level Sensor - NO, Active LOW |
| **PA2** | `OVERFLOW_SENSOR` | Input | Overflow Sensor (Optional) - NO, Active LOW |

### STM32CubeMX Configuration

When configuring in STM32CubeMX, use these exact labels for each pin:

**Outputs:**
- PC13: **`LED_PROGRAM_RUNNING`** (GPIO_Output)
- PC14: **`LED_STATUS_WATER_GALLON`** (GPIO_Output)
- PC15: **`PUMP_WATER_GALLON`** (GPIO_Output)

**Inputs:**
- PA0: **`DOOR_SW`** (GPIO_Input, Pull-up)
- PA1: **`WATER_LIMIT`** (GPIO_Input, Pull-up)
- PA2: **`OVERFLOW_SENSOR`** (GPIO_Input, Pull-up) - Only if using overflow sensor

---

## Configuration Guide (`config.h`)

All system settings are located in [config.h](file:///d:/tes/KON/Core/Inc/config.h).

### 1. LED Polarity Configuration

```c
/* Program LED (Heartbeat) */
#define PROGRAM_LED_ACTIVE_LOW   // Active Low (Current setting)
// #define PROGRAM_LED_ACTIVE_HIGH

/* Status LED (Water/Fill) */
// #define STATUS_LED_ACTIVE_LOW
#define STATUS_LED_ACTIVE_HIGH   // Active High (Current setting)
```

### 2. Pump/Relay Configuration

```c
// #define PUMP_ACTIVE_LOW         // For most 5V relay modules
#define PUMP_ACTIVE_HIGH        // Current setting
```

### 3. Door Switch Configuration

```c
#define DOOR_SWITCH_TYPE_NO     // Normally Open (Current)
// #define DOOR_SWITCH_TYPE_NC  // Normally Closed

#define DOOR_SWITCH_ACTIVE_LOW  // Current setting
// #define DOOR_SWITCH_ACTIVE_HIGH
```

### 4. Water Level Sensor Configuration

```c
#define WATER_SENSOR_TYPE_NO    // Normally Open (Current)
// #define WATER_SENSOR_TYPE_NC // Normally Closed

#define WATER_SENSOR_ACTIVE_LOW // Current setting
// #define WATER_SENSOR_ACTIVE_HIGH
```

### 5. Overflow Sensor Configuration (Optional)

```c
#define OVERFLOW_SENSOR_TYPE_NO    // Normally Open (Current)
// #define OVERFLOW_SENSOR_TYPE_NC

#define OVERFLOW_SENSOR_ACTIVE_LOW // Current setting
// #define OVERFLOW_SENSOR_ACTIVE_HIGH
```

---

## Timing Configuration

### Pump Operation Timings

```c
#define PUMP_NORMAL_FILL_TIME   360000  // 6 minutes (360000 ms)
#define PUMP_MAX_RUN_TIME       540000  // 9 minutes (540000 ms)
#define MIN_PUMP_INTERVAL       10000   // 10 seconds between cycles
#define PUMP_STARTUP_DELAY      2000    // 2 seconds after door closes
```

**How to adjust:**
- Measure actual fill time from empty to full.
- Set `PUMP_NORMAL_FILL_TIME` to 1.2x - 1.5x measured time.
- Set `PUMP_MAX_RUN_TIME` to 1.5x `PUMP_NORMAL_FILL_TIME`.

### Sensor Debounce

```c
#define DEBOUNCE_DELAY          100     // 100 ms
```

Increase if sensors are triggering falsely (noisy environment).

### Pump Protection (New in v2.1)

```c
#define MAX_PUMP_DUTY_CYCLE     30      // Max 30% duty cycle over 10 mins
#define PUMP_OVERHEAT_COOLDOWN  300000  // 5 min forced cooldown
```

**How it works:**
- System tracks pump usage over a 10-minute window.
- If pump runs >30% of the time (3 mins in 10 mins), it forces a cooldown.
- Prevents motor overheating during heavy usage.

### LED Blink Rates

```c
#define LED_BLINK_FAST          250     // Fast blink: 250 ms
#define LED_BLINK_SLOW          500     // Slow blink: 500 ms
#define LED_BLINK_ERROR         125     // Error blink: 125 ms
```

---

## Feature Enable/Disable

### Basic Features

```c
#define ENABLE_COOLDOWN_PERIOD  1       // 1 = ON, 0 = OFF
#define ENABLE_STARTUP_DELAY    1       // 1 = ON, 0 = OFF
#define ENABLE_TIMEOUT_SAFETY   1       // 1 = ON, 0 = OFF (Recommended: ON)
#define ENABLE_RAPID_CYCLE_CHECK 1      // 1 = ON, 0 = OFF (Recommended: ON)
```

### Optional Overflow Sensor

```c
#define ENABLE_OVERFLOW_SENSOR  0       // 1 = Enable, 0 = Disable (Default)
```

**To enable:**
1. Wire sensor to PA2.
2. Change to `#define ENABLE_OVERFLOW_SENSOR  1`.
3. Recompile and flash.

---

## Common Configuration Scenarios

### Scenario 1: Standard Relay Module (Most Common)

```c
#define PROGRAM_LED_ACTIVE_LOW
#define STATUS_LED_ACTIVE_HIGH
#define PUMP_ACTIVE_LOW              // Change to ACTIVE_LOW
#define DOOR_SWITCH_TYPE_NO
#define DOOR_SWITCH_ACTIVE_LOW
#define WATER_SENSOR_TYPE_NO
#define WATER_SENSOR_ACTIVE_LOW
#define ENABLE_OVERFLOW_SENSOR  0
```

### Scenario 2: With Overflow Sensor

```c
// Same as Scenario 1, but add:
#define ENABLE_OVERFLOW_SENSOR  1
```

### Scenario 3: All Active HIGH System

```c
#define PROGRAM_LED_ACTIVE_HIGH
#define STATUS_LED_ACTIVE_HIGH
#define PUMP_ACTIVE_HIGH
#define DOOR_SWITCH_TYPE_NO
#define DOOR_SWITCH_ACTIVE_HIGH
#define WATER_SENSOR_TYPE_NO
#define WATER_SENSOR_ACTIVE_HIGH
```

---

## Error Codes Reference

| Code | Error Name | Trigger Condition | How to Reset |
|------|-----------|-------------------|--------------|
| **0** | No Error | Normal operation | - |
| **1** | Pump Timeout | Pump ran > 9 minutes | Open door for 3+ seconds |
| **2** | Sensor Fault | Sensor logic error | Open door for 3+ seconds |
| **3** | Rapid Cycling | >10 cycles, avg < 30s | Open door for 3+ seconds |
| **4** | Gallon Empty | Pump ran 6 mins, tank not full | Open door, change gallon, close door |
| **5** | Overflow | Overflow sensor triggered | Open door for 3+ seconds |

---

## How to Modify Settings

### Step 1: Open `config.h`
Location: `d:\tes\KON\Core\Inc\config.h`

### Step 2: Find the Setting
Use the sections above to locate the setting you want to change.

### Step 3: Comment/Uncomment
- To enable: Remove `//` from the line.
- To disable: Add `//` at the start of the line.

Example:
```c
// Before (Pump Active LOW)
#define PUMP_ACTIVE_LOW
// #define PUMP_ACTIVE_HIGH

// After (Pump Active HIGH)
// #define PUMP_ACTIVE_LOW
#define PUMP_ACTIVE_HIGH
```

### Step 4: Recompile and Flash
Build the project in STM32CubeIDE and flash to the board.

---

## Troubleshooting

### Problem: Pump won't turn on
**Check:**
1. `PUMP_ACTIVE_LOW` vs `PUMP_ACTIVE_HIGH` setting matches your relay.
2. Door is closed.
3. Tank is not already full.

### Problem: LEDs not working correctly
**Check:**
1. `PROGRAM_LED_ACTIVE_LOW` vs `ACTIVE_HIGH` setting.
2. `STATUS_LED_ACTIVE_LOW` vs `ACTIVE_HIGH` setting.

### Problem: Sensor always reads "Full" or "Empty"
**Check:**
1. `WATER_SENSOR_TYPE_NO` vs `NC` matches your sensor.
2. `WATER_SENSOR_ACTIVE_LOW` vs `HIGH` setting.
3. Physical wiring and sensor functionality.

### Problem: Getting Error 4 (Gallon Empty) too soon
**Increase:**
```c
#define PUMP_NORMAL_FILL_TIME   420000  // Increase to 7 minutes
```

---

## Support & Modifications

For questions or custom modifications, refer to:
- [documentation.md](file:///C:/Users/Cuplis%20Kei%20Darma/.gemini/antigravity/brain/9be6d230-97fc-4e97-8885-1998013bbcf6/documentation.md) - Technical documentation
- [config.h](file:///d:/tes/KON/Core/Inc/config.h) - All configuration options
