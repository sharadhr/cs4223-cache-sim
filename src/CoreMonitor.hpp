//
// Created by sharadh on 22/10/21.
//

#ifndef CS4223_CACHE_SIM_COREMONITOR_HPP
#define CS4223_CACHE_SIM_COREMONITOR_HPP

#include <array>

namespace CacheSim {
class CoreMonitor {
 public:

 private:
  std::array<uint32_t, 4> executionCycles;
  std::array<uint32_t, 4> computeCycles;
  std::array<uint32_t, 4> loadStoreInst;
  std::array<uint32_t, 4> idleCycles;

  friend class Processor;
};
}// namespace CacheSim

#endif//CS4223_CACHE_SIM_COREMONITOR_HPP
