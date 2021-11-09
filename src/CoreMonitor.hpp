//
// Created by sharadh on 22/10/21.
//

#ifndef CS4223_CACHE_SIM_COREMONITOR_HPP
#define CS4223_CACHE_SIM_COREMONITOR_HPP

#include <cstdint>

namespace CacheSim {
class CoreMonitor {
 public:

 private:
  uint64_t executionCycleCount{};
  uint64_t computeCycleCount{};
  uint64_t idleCycleCount{};
  uint64_t loadStoreCount{};

  friend class Processor;
};
}// namespace CacheSim

#endif//CS4223_CACHE_SIM_COREMONITOR_HPP
