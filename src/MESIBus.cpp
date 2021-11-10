#include <algorithm>

#include "Bus.hpp"
#include "Processor.hpp"

namespace CacheSim {
/* MESIBus::MESIBus(std::array<Processor, 4>& processors) : Bus(processors) {} */

uint32_t MESIBus::cyclesToWaitFor(uint8_t pid, uint32_t address) {
  uint32_t blockNum = address / blockSize;
  int minCycles = 100;
  for (auto& processor : processors) {
    if (processor.pid == pid) continue;

    if (processor.cache.has(address)) {
      minCycles = std::min<int>(minCycles, 2 * blockSize / 4);
    } else if (processor.cache.blockedOnAddress / blockSize == blockNum) {
      minCycles = std::min<int>(minCycles, processor.cache.blockedFor + 2 * blockSize / 4);
    }
  }
  return minCycles;
}
}// namespace CacheSim
