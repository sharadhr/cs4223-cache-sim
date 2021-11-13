#include "Bus.hpp"

namespace CacheSim {
uint32_t DragonBus::getBlockedCycles(std::array<std::shared_ptr<Cache>, 4>&& caches, uint8_t pid, CacheOp cacheOp,
                                     uint32_t address) {
  switch (cacheOp) {
    case CacheOp::PR_WB:
    case CacheOp::PR_NULL:
      return 0;
    case CacheOp::PR_RD_HIT:
      return 1;
    case CacheOp::PR_WR_HIT:
      return 1;
    case CacheOp::PR_RD_MISS:
    case CacheOp::PR_WR_MISS:
      bool anyCacheContains = false;
      for (int i = 0; i < 4; i++) {
        if (caches[i]->containsAddress(address)) anyCacheContains = true;
      }
      return anyCacheContains ? 2 : 100;
  }
}

void DragonBus::transition(std::array<std::shared_ptr<Cache>, 4>&& caches, uint8_t pid) {}
}// namespace CacheSim
