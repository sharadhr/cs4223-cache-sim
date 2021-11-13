#pragma once

#include <cstdint>

namespace CacheSim {
class CoreMonitor {
 public:

 private:
  uint64_t executionCycleCount{};
  uint64_t cycleCount{};
  uint64_t idleCycleCount{};
  uint64_t loadStoreCount{};

  friend class Processor;
};
}// namespace CacheSim
