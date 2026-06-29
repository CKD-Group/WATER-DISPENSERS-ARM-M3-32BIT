/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : sensors.h
  * @brief          : Sensors module header for Water Dispenser Control
  * @author         : Cuplis Kei Darma
  ******************************************************************************
  * This file contains sensor reading functions with debouncing
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __SENSORS_H
#define __SENSORS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "config.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/

/**
  * @brief  Initialize sensors module
  * @param  None
  * @retval None
  */
void Sensors_Init(void);

/**
  * @brief  Read door switch status (debounced)
  * @param  None
  * @retval uint8_t 1 if door closed, 0 if door open
  */
uint8_t Sensors_IsDoorClosed(void);

/**
  * @brief  Read door switch status (raw, no debounce)
  * @param  None
  * @retval uint8_t 1 if door closed, 0 if door open
  */
uint8_t Sensors_IsDoorClosedRaw(void);

/**
  * @brief  Read water level sensor - tank full (debounced)
  * @param  None
  * @retval uint8_t 1 if tank full, 0 if not full
  */
uint8_t Sensors_IsTankFull(void);

/**
  * @brief  Read water level sensor - tank empty (debounced)
  * @param  None
  * @retval uint8_t 1 if tank empty, 0 if not empty
  */
uint8_t Sensors_IsTankEmpty(void);

/**
  * @brief  Read overflow sensor (if enabled)
  * @param  None
  * @retval uint8_t 1 if overflow detected, 0 if normal
  */
uint8_t Sensors_IsOverflow(void);

/**
  * @brief  Read water level sensor (raw, no debounce)
  * @param  None
  * @retval GPIO_PinState Raw pin state
  */
GPIO_PinState Sensors_ReadWaterLevelRaw(void);

/**
  * @brief  Generic debounced GPIO read function
  * @param  GPIOx GPIO port
  * @param  GPIO_Pin GPIO pin
  * @retval GPIO_PinState Debounced pin state
  */
GPIO_PinState Sensors_DebouncedRead(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

/**
  * @brief  Test all sensors (for diagnostics)
  * @param  None
  * @retval uint8_t Test result: 0 = all OK, error code otherwise
  */
uint8_t Sensors_SelfTest(void);

#ifdef __cplusplus
}
#endif

#endif /* __SENSORS_H */
