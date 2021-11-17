#pragma once

#include <cstdint>

namespace CacheSim {
class CoreMonitor {
 public:
  uint64_t executionCycleCount{};
  uint64_t cycleCount{};
  uint64_t idleCycleCount{};
  uint64_t loadStoreCount{};
  uint32_t missCount{};
  uint32_t hitCount{};

  friend class Processor;
};
}// namespace CacheSim
