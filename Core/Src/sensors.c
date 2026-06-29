/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : sensors.c
  * @brief          : Sensors module implementation for Water Dispenser Control
  * @author         : Cuplis Kei Darma
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "sensors.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Initialize sensors module
  * @param  None
  * @retval None
  */
void Sensors_Init(void)
{
  // Sensor GPIOs are already initialized by MX_GPIO_Init()
  // This function reserved for future sensor initialization if needed

  // Could add initial sensor check here
  HAL_Delay(100);  // Allow sensors to stabilize after power-on
}

/**
  * @brief  Read door switch status (debounced)
  * @param  None
  * @retval uint8_t 1 if door closed, 0 if door open
  */
uint8_t Sensors_IsDoorClosed(void)
{
  return IS_DOOR_CLOSED();
}

/**
  * @brief  Read door switch status (raw, no debounce)
  * @param  None
  * @retval uint8_t 1 if door closed, 0 if door open
  */
uint8_t Sensors_IsDoorClosedRaw(void)
{
  GPIO_PinState pinState = HAL_GPIO_ReadPin(DOOR_SW_GPIO_Port, DOOR_SW_Pin);

  #if defined(DOOR_SWITCH_TYPE_NO) && defined(DOOR_SWITCH_ACTIVE_LOW)
    return (pinState == GPIO_PIN_RESET) ? 1 : 0;
  #elif defined(DOOR_SWITCH_TYPE_NO) && defined(DOOR_SWITCH_ACTIVE_HIGH)
    return (pinState == GPIO_PIN_SET) ? 1 : 0;
  #elif defined(DOOR_SWITCH_TYPE_NC) && defined(DOOR_SWITCH_ACTIVE_LOW)
    return (pinState == GPIO_PIN_SET) ? 1 : 0;
  #elif defined(DOOR_SWITCH_TYPE_NC) && defined(DOOR_SWITCH_ACTIVE_HIGH)
    return (pinState == GPIO_PIN_RESET) ? 1 : 0;
  #endif
}

/**
  * @brief  Read water level sensor - tank full (debounced)
  * @param  None
  * @retval uint8_t 1 if tank full, 0 if not full
  */
uint8_t Sensors_IsTankFull(void)
{
  return IS_TANK_FULL();
}

uint8_t Sensors_IsTankEmpty(void)
{
  return IS_TANK_EMPTY();
}

/**
  * @brief  Read overflow sensor (if enabled)
  * @param  None
  * @retval uint8_t 1 if overflow detected, 0 if normal
  */
uint8_t Sensors_IsOverflow(void)
{
  #if ENABLE_OVERFLOW_SENSOR
    GPIO_PinState pinState = Sensors_DebouncedRead(OVERFLOW_SENSOR_GPIO_Port, OVERFLOW_SENSOR_Pin);

    #if defined(OVERFLOW_SENSOR_TYPE_NO) && defined(OVERFLOW_SENSOR_ACTIVE_LOW)
      return (pinState == GPIO_PIN_RESET) ? 1 : 0;
    #elif defined(OVERFLOW_SENSOR_TYPE_NO) && defined(OVERFLOW_SENSOR_ACTIVE_HIGH)
      return (pinState == GPIO_PIN_SET) ? 1 : 0;
    #elif defined(OVERFLOW_SENSOR_TYPE_NC) && defined(OVERFLOW_SENSOR_ACTIVE_LOW)
      return (pinState == GPIO_PIN_SET) ? 1 : 0;
    #elif defined(OVERFLOW_SENSOR_TYPE_NC) && defined(OVERFLOW_SENSOR_ACTIVE_HIGH)
      return (pinState == GPIO_PIN_RESET) ? 1 : 0;
    #endif
  #else
    return 0; // Always return 0 (no overflow) if sensor disabled
  #endif
}

/**
  * @brief  Read water level sensor (raw, no debounce)
  * @param  None
  * @retval GPIO_PinState Raw pin state
  */
GPIO_PinState Sensors_ReadWaterLevelRaw(void)
{
  return HAL_GPIO_ReadPin(WATER_LIMIT_GPIO_Port, WATER_LIMIT_Pin);
}

/**
  * @brief  Generic debounced GPIO read function
  * @param  GPIOx GPIO port
  * @param  GPIO_Pin GPIO pin
  * @retval GPIO_PinState Debounced pin state
  */
GPIO_PinState Sensors_DebouncedRead(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  GPIO_PinState state1 = HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);
  HAL_Delay(DEBOUNCE_DELAY);
  GPIO_PinState state2 = HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);

  // If both reads are the same, return that state
  if(state1 == state2) {
    return state1;
  }

  // If different, read one more time to confirm
  return HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);
}

/**
  * @brief  Test all sensors (for diagnostics)
  * @param  None
  * @retval uint8_t Test result: 0 = all OK, non-zero = error code
  */
uint8_t Sensors_SelfTest(void)
{
  uint8_t errorCode = 0;

  // Test door switch
  GPIO_PinState doorState = HAL_GPIO_ReadPin(DOOR_SW_GPIO_Port, DOOR_SW_Pin);
  if(doorState != GPIO_PIN_RESET && doorState != GPIO_PIN_SET) {
    errorCode |= 0x01;  // Door sensor error
  }

  // Test water level sensor
  GPIO_PinState waterState = HAL_GPIO_ReadPin(WATER_LIMIT_GPIO_Port, WATER_LIMIT_Pin);
  if(waterState != GPIO_PIN_RESET && waterState != GPIO_PIN_SET) {
    errorCode |= 0x02;  // Water sensor error
  }

  return errorCode;
}

/* Private functions ---------------------------------------------------------*/
