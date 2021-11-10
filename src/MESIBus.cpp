#include <algorithm>

#include "Bus.hpp"
#include "Processor.hpp"

namespace CacheSim {
uint32_t MESIBus::cyclesToWaitFor(std::array<Processor, 4>& processors, uint32_t pid, CacheOp blockOp) {
  uint32_t blockNum = processors[pid].cache.blockedOnAddress / blockSize;
  int minCycles = 100;
  for (auto i = 0; i < (int) processors.size(); i++) {
    if (i == (int) pid) continue;

    if (processors[i].cache.has(processors[pid].cache.blockedOnAddress)) {
      minCycles = std::min<int>(minCycles, 2 * blockSize / 4);
    } else if (processors[i].cache.isBlocked && processors[i].cache.blockedOnAddress / blockSize == blockNum) {
      minCycles = std::min<int>(minCycles, processors[i].cache.blockedFor + 2 * blockSize / 4);
    }
  }
  return minCycles;
}
}// namespace CacheSim
