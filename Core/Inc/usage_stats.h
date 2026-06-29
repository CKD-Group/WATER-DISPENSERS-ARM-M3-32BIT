#ifndef USAGE_STATS_H
#define USAGE_STATS_H

#include "main.h"

// Define this in config.h to enable
#ifndef ENABLE_USAGE_STATS
#define ENABLE_USAGE_STATS 0
#endif

typedef struct {
  uint32_t totalLitersPumped; // Estimated
  uint32_t totalFills;
  uint32_t totalRuntimeSec;
} UsageStats_t;

void UsageStats_Init(void);
void UsageStats_Update(uint32_t fillDurationMs);
UsageStats_t* UsageStats_Get(void);

#endif // USAGE_STATS_H
