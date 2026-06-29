#include "battery_monitor.h"
#include "config.h"

#if ENABLE_BATTERY_MONITOR

// Assumes ADC1 is initialized and handle is hadc1
extern ADC_HandleTypeDef hadc1;

#define BATTERY_LOW_THRESHOLD_MV  3000  // 3.0V
#define VREF_MV                   3300  // 3.3V
#define ADC_RESOLUTION            4095  // 12-bit
#define VOLTAGE_DIVIDER_RATIO     2     // e.g. 10k/10k divider

/**
  * @brief  Read battery voltage in millivolts
  * @retval Voltage in mV
  */
uint16_t Battery_GetVoltage_mV(void)
{
  // Start ADC Conversion
  HAL_ADC_Start(&hadc1);
  
  // Wait for conversion to complete
  if (HAL_ADC_PollForConversion(&hadc1, 10) != HAL_OK) {
    return 0; // Error
  }
  
  // Read value
  uint32_t adcValue = HAL_ADC_GetValue(&hadc1);
  
  // Calculate voltage
  // V = (ADC / Resolution) * Vref * Divider
  uint16_t voltage_mV = (adcValue * VREF_MV * VOLTAGE_DIVIDER_RATIO) / ADC_RESOLUTION;
  
  return voltage_mV;
}

/**
  * @brief  Check battery status and take action if low
  */
void Battery_Check(void)
{
  uint16_t voltage = Battery_GetVoltage_mV();
  
  if(voltage < BATTERY_LOW_THRESHOLD_MV) {
    // Low battery warning
    // Could set a flag, blink an LED, or enter low power mode
    // For now, just a placeholder
  }
}

#else

// Stubs if disabled
uint16_t Battery_GetVoltage_mV(void) { return 0; }
void Battery_Check(void) {}

#endif // ENABLE_BATTERY_MONITOR
