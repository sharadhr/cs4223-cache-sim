
#include "Bus.hpp"

namespace CacheSim {
uint32_t MESIBus::getBlockedCycles(std::array<std::shared_ptr<Cache>, 4>&& caches, uint8_t pid, CacheOp) { return {}; }

void MESIBus::transition(std::array<std::shared_ptr<Cache>, 4>&& caches, uint8_t pid) {}
}// namespace CacheSim
