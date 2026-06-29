# Water Dispenser Control System

Automated water dispenser control system for STM32F103C8T6 (Blue Pill) with intelligent pump management, safety features, and LED status indicators.

## Features

- ✅ **Automatic Fill Control** - Monitors water level and automatically refills when empty
- ✅ **Door Safety Interlock** - Pump immediately stops when door opens
- ✅ **Gallon Empty Detection** - Specific error when source gallon runs out
- ✅ **Overflow Protection** - Optional secondary sensor prevents overflow
- ✅ **LED Status Indicators** - Clear visual feedback of system state
- ✅ **Configurable Hardware** - Supports Active HIGH/LOW for all components
- ✅ **Safety Timeouts** - Multiple layers of pump protection
- ✅ **Error Recovery** - Simple door-open reset for all errors
- ✅ **Efficiency Optimized** - Non-blocking loop, adaptive rate, and motor protection
- ✅ **Smart Diagnostics** - Pump health tracking and duty cycle monitoring

## Hardware Requirements

### Microcontroller
- STM32F103C8T6 (Blue Pill)

### Components
- Water pump or relay module
- Door switch (NO/NC supported)
- Water level sensor (float switch)
- 2x LEDs (Program and Status indicators)
- Optional: Overflow sensor (for redundancy)

## LED Patterns

The system uses two LEDs to communicate status:

| System State | Program LED (PC13) | Status LED (PC14) | Meaning |
|--------------|-------------------|-------------------|---------|
| **IDLE** | 🟢 **ON** | ⚫ OFF | Ready, waiting |
| **DOOR OPEN** | 🔴 **Fast Blink** (250ms) | ⚫ OFF | Close door to proceed |
| **WAIT SETTLE** | 🟢 **ON** | 🟡 **Slow Blink** (500ms) | Waiting for water to settle |
| **FILLING** | 🟢 **ON** | 🔴 **Fast Blink** (250ms) | Pump active, filling tank |
| **FULL** | 🟢 **ON** | 🟢 **ON** | Tank full and ready |
| **COOLDOWN** | 🟢 **ON** | 🟡 **Slow Blink** (500ms) | Pump resting between cycles |
| **ERROR** | 🔴 **Fast Blink** | 🔴 **Fast Blink** | Critical error (see error codes) |

### Blink Rates
- **Fast Blink**: 250ms ON / 250ms OFF (active states)
- **Slow Blink**: 500ms ON / 500ms OFF (passive states)
- **Error Blink**: 125ms ON / 125ms OFF (error indication)

## Pinout

| Pin | Label | Type | Function |
|-----|-------|------|----------|
| PC13 | `LED_PROGRAM_RUNNING` | Output | System heartbeat LED (Active LOW) |
| PC14 | `LED_STATUS_WATER_GALLON` | Output | Water status LED (Active HIGH) |
| PC15 | `PUMP_WATER_GALLON` | Output | Pump/Relay control (Active HIGH) |
| PA0 | `DOOR_SW` | Input | Door switch (NO, Active LOW) |
| PA1 | `WATER_LIMIT` | Input | Water level sensor (NO, Active LOW) |
| PA2 | `OVERFLOW_SENSOR` | Input | Overflow sensor - Optional (NO, Active LOW) |

## Quick Start

### 1. Clone Repository
```bash
git clone <repository-url>
cd water-dispenser-control
```

### 2. Open in STM32CubeIDE
- Import project into STM32CubeIDE
- Build the project

### 3. Configure Hardware (Optional)
Edit `Core/Inc/config.h` to match your hardware:
```c
// LED Polarity
#define PROGRAM_LED_ACTIVE_LOW   // or ACTIVE_HIGH
#define STATUS_LED_ACTIVE_HIGH   // or ACTIVE_LOW

// Pump/Relay
#define PUMP_ACTIVE_HIGH         // or ACTIVE_LOW

// Sensors
#define WATER_SENSOR_TYPE_NO     // or TYPE_NC
#define WATER_SENSOR_ACTIVE_LOW  // or ACTIVE_HIGH
```

### 4. Flash to STM32
- Connect ST-Link programmer
- Flash firmware to STM32F103C8T6

### 5. Initial Calibration
- Measure actual fill time from empty to full
- Update timing in `config.h`:
```c
#define PUMP_NORMAL_FILL_TIME   360000  // Adjust based on measurement
```

## Configuration

All settings are in [`Core/Inc/config.h`](Core/Inc/config.h).

### Key Settings

**Pump Timings:**
```c
#define PUMP_NORMAL_FILL_TIME   360000  // 6 minutes
#define PUMP_MAX_RUN_TIME       540000  // 9 minutes (safety)
#define MIN_PUMP_INTERVAL       10000   // 10 seconds between cycles
```

**Optional Features:**
```c
#define ENABLE_COOLDOWN_PERIOD  1       // Pump rest between cycles
#define ENABLE_OVERFLOW_SENSOR  0       // Secondary overflow sensor (default OFF)
```

See [USER_GUIDE.md](USER_GUIDE.md) for complete configuration options.

## Error Codes

| Code | Name | Description | Reset Method |
|------|------|-------------|--------------|
| 0 | No Error | Normal operation | - |
| 1 | Pump Timeout | Pump ran > 9 minutes | Open door 3+ seconds |
| 2 | Sensor Fault | Sensor logic error | Open door 3+ seconds |
| 3 | Rapid Cycling | Pump cycling too fast | Open door 3+ seconds |
| 4 | **Gallon Empty** | Source gallon is empty | Open door, change gallon, close door |
| 5 | Overflow | Overflow sensor triggered | Open door 3+ seconds |

### Error 4: Gallon Empty (Most Common)

**How it works:**
1. Pump runs for 6 minutes (normal fill time)
2. Tank still not full → Error 4 triggered
3. Both LEDs blink fast

**How to reset:**
1. Open door
2. Replace empty gallon with full one
3. Close door
4. System resumes automatically

## State Machine

```
┌──────────┐
│   IDLE   │ ←──────────────────┐
└────┬─────┘                    │
     │ Tank Empty               │
     ▼                          │
┌──────────────┐                │
│ WAIT_SETTLE  │                │
└──────┬───────┘                │
       │                        │
       ▼                        │
┌──────────┐                    │
│ FILLING  │                    │
└────┬─────┘                    │
     │ Tank Full                │
     ▼                          │
┌──────────┐                    │
│   FULL   │                    │
└────┬─────┘                    │
     │ Water Consumed           │
     ▼                          │
┌────────────┐                  │
│ COOLDOWN   │─────────────────┘
└────────────┘

    Any State ───[Door Open]───> DOOR_OPEN ───[Door Close]───> WAIT_SETTLE
    Any State ───[Error]────────> ERROR ───[Door Open 3s]────> IDLE
```

## Safety Features

1. **Door Interlock** - Pump stops immediately if door opens
2. **Pump Timeout** - Hard limit at 9 minutes (prevents motor burnout)
3. **Gallon Empty Detection** - Stops at 6 minutes if tank not full
4. **Rapid Cycling Protection** - Prevents pump damage from sensor faults
5. **Debouncing** - Software debounce (100ms) prevents false triggers
6. **Optional Overflow Sensor** - Secondary protection against overflow

## Project Structure

```
Core/
├── Inc/
│   ├── config.h          # ⚙️ Main configuration file
│   ├── main.h            # Pin definitions
│   ├── state_machine.h   # FSM header
│   └── sensors.h         # Sensor abstraction
├── Src/
│   ├── main.c            # Main program
│   ├── state_machine.c   # Core logic
│   └── sensors.c         # Sensor handling
└── Startup/
    └── startup_stm32f103c8tx.s
```

## Documentation

- [USER_GUIDE.md](USER_GUIDE.md) - Complete configuration and troubleshooting guide
- [documentation.md](documentation.md) - Technical documentation

## Common Use Cases

### Scenario 1: Standard 5V Relay Module
```c
#define PUMP_ACTIVE_LOW     // Most relay modules are active LOW
```

### Scenario 2: Add Overflow Protection
```c
#define ENABLE_OVERFLOW_SENSOR  1   // Enable
// Wire sensor to PA2
```

### Scenario 3: Faster Fill Time
```c
#define PUMP_NORMAL_FILL_TIME   240000  // Reduce to 4 minutes
#define PUMP_MAX_RUN_TIME       360000  // Adjust max accordingly
```

## Troubleshooting

### Pump won't start
- Check door is closed
- Verify `PUMP_ACTIVE_LOW` vs `PUMP_ACTIVE_HIGH` setting
- Check tank is not already full

### False "Gallon Empty" errors
- Increase `PUMP_NORMAL_FILL_TIME` in config.h
- Check for air in suction hose

### LEDs not working
- Verify `LED_ACTIVE_LOW` vs `LED_ACTIVE_HIGH` settings
- Check LED connections

See [USER_GUIDE.md](USER_GUIDE.md) for detailed troubleshooting.

## License

This project is provided as-is for educational and commercial use.

## Author

Cuplis Kei Darma

---

**Need Help?** Check [USER_GUIDE.md](USER_GUIDE.md) or open an issue.
