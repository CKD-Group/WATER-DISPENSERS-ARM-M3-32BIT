#include "usage_stats.h"
#include "config.h"

#if ENABLE_USAGE_STATS

static UsageStats_t stats;

// Estimated flow rate in mL per second
// Adjust based on pump spec
#define PUMP_FLOW_RATE_ML_PER_SEC  20 

/**
  * @brief  Initialize usage statistics
  */
void UsageStats_Init(void)
{
  // TODO: Load from persistent storage
  stats.totalLitersPumped = 0;
  stats.totalFills = 0;
  stats.totalRuntimeSec = 0;
}

/**
  * @brief  Update statistics after a fill cycle
  * @param  fillDurationMs Duration of the fill in milliseconds
  */
void UsageStats_Update(uint32_t fillDurationMs)
{
  stats.totalFills++;
  
  uint32_t seconds = fillDurationMs / 1000;
  stats.totalRuntimeSec += seconds;
  
  // Calculate volume
  uint32_t mlPumped = seconds * PUMP_FLOW_RATE_ML_PER_SEC;
  
  // Update total liters (accumulate mL then convert to avoid float)
  // Simplified for this example
  stats.totalLitersPumped += (mlPumped / 1000);
  
  // TODO: Save to persistent storage
}

/**
  * @brief  Get current statistics
  */
UsageStats_t* UsageStats_Get(void)
{
  return &stats;
}

#else

// Stubs
void UsageStats_Init(void) {}
void UsageStats_Update(uint32_t fillDurationMs) {}
UsageStats_t* UsageStats_Get(void) { return NULL; }

#endif // ENABLE_USAGE_STATS
