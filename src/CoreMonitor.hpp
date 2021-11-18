#pragma once

#include <cstdint>

namespace CacheSim {
struct CoreMonitor {
  uint64_t cycleCount{};
  uint64_t computeCycleCount{};
  uint64_t idleCycleCount{};
  uint64_t loadCount{};
  uint64_t storeCount{};
  uint64_t missCount{};
  uint64_t hitCount{};
};
}// namespace CacheSim
