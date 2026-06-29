#ifndef CONFIG_STORAGE_H
#define CONFIG_STORAGE_H

#include "main.h"

HAL_StatusTypeDef Config_Save(void);
HAL_StatusTypeDef Config_Restore(void);

#endif // CONFIG_STORAGE_H
