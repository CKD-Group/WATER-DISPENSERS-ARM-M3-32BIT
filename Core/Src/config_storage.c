#include "config_storage.h"
#include "config.h"

// STM32F103C8T6: 64KB flash, last page at 0x0800F800
#define CONFIG_FLASH_ADDR   0x0800F800

typedef struct {
  uint32_t magic;  // 0xC0FF1CE5 - validates config
  uint16_t version;
  
  // Store critical config
  uint32_t pumpNormalFillTime;
  uint32_t pumpMaxRunTime;
  uint16_t minPumpInterval;
  
  uint32_t crc;  // CRC32 of entire structure
} StoredConfig_t;

/**
  * @brief  Save configuration to flash
  */
HAL_StatusTypeDef Config_Save(void)
{
  StoredConfig_t config;
  
  config.magic = 0xC0FF1CE5;
  config.version = 0x0200;  // v2.0
  config.pumpNormalFillTime = PUMP_NORMAL_FILL_TIME;
  config.pumpMaxRunTime = PUMP_MAX_RUN_TIME;
  config.minPumpInterval = MIN_PUMP_INTERVAL;
  
  // Calculate CRC (simplified - use HAL CRC if available)
  config.crc = 0;  // TODO: Calculate CRC32
  
  // Unlock flash
  HAL_FLASH_Unlock();
  
  // Erase page
  FLASH_EraseInitTypeDef eraseInit;
  uint32_t pageError;
  
  eraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
  eraseInit.PageAddress = CONFIG_FLASH_ADDR;
  eraseInit.NbPages = 1;
  
  if(HAL_FLASHEx_Erase(&eraseInit, &pageError) != HAL_OK) {
    HAL_FLASH_Lock();
    return HAL_ERROR;
  }
  
  // Write config (word by word)
  uint32_t* data = (uint32_t*)&config;
  for(int i = 0; i < sizeof(StoredConfig_t)/4; i++) {
    if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, 
                         CONFIG_FLASH_ADDR + (i*4), 
                         data[i]) != HAL_OK) {
      HAL_FLASH_Lock();
      return HAL_ERROR;
    }
  }
  
  HAL_FLASH_Lock();
  return HAL_OK;
}

/**
  * @brief  Restore configuration from flash
  */
HAL_StatusTypeDef Config_Restore(void)
{
  StoredConfig_t* config = (StoredConfig_t*)CONFIG_FLASH_ADDR;
  
  // Validate magic number
  if(config->magic != 0xC0FF1CE5) {
    return HAL_ERROR;  // No valid config
  }
  
  // TODO: Validate CRC
  
  // Apply config (requires recompilation with new values or variable-based config)
  // This is more for diagnostics than runtime config in this version
  
  return HAL_OK;
}
