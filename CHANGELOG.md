# Changelog

## [v2.1.0] - Efficiency Update

### ⚡ CPU & Power Optimization
- **Non-Blocking Main Loop**: Replaced `HAL_Delay` with adaptive time-based polling.
- **Adaptive Loop Rate**:
  - FILLING: 10ms (Fast response)
  - ERROR/DOOR_OPEN: 20ms (Medium response)
  - IDLE/FULL: 50ms (Power saving)
- **IWDG Refresh**: Added non-blocking watchdog refresh every 3 seconds.
- **EXTI Debounce**: Added 50ms software debounce to all sensor interrupts.

### 🔧 Motor Efficiency & Protection
- **Duty Cycle Protection**: Limits pump usage to 30% over 10-minute window.
- **Overheat Cooldown**: Enforces 5-minute cooldown if duty cycle exceeded.
- **Runtime Tracking**:
  - Tracks average, longest, and shortest pump runs.
  - Calculates pump health score (0-100).

### 🐛 Bug Fixes
- Fixed potential blocking delay issues.
- Reduced false triggers from noisy sensors.
