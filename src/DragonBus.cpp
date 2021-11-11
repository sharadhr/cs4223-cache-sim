#include "Bus.hpp"

namespace CacheSim {
uint32_t DragonBus::blockedCycles(std::array<std::shared_ptr<Cache>, 4>& caches, uint8_t pid, CacheOp) { return {}; }

void DragonBus::transition(std::array<std::shared_ptr<Cache>, 4>& caches, uint8_t pid) {}
}// namespace CacheSim
