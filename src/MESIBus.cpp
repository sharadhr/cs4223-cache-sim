
#include "Bus.hpp"

namespace CacheSim {
std::array<uint32_t, 4> MESIBus::blockCycles(std::array<uint32_t, 4> &currentCycles, uint8_t pid, CacheOp) {
  return {};
}

std::array<CacheLine::CacheState, 4> MESIBus::transition(std::array<CacheLine::CacheState, 4>& previousCycles,
                                                         uint8_t pid, CacheOp operation) {
  return {};
}
}// namespace CacheSim
