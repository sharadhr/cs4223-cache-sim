
#include <algorithm>

#include "Bus.hpp"

namespace CacheSim {
uint32_t MESIBus::getBlockedCycles(std::array<std::shared_ptr<Cache>, 4>&& caches, CacheOp cacheOp, uint32_t address,
                                   uint8_t drop_pid) {
  switch (cacheOp) {
    case CacheOp::PR_WB:
    case CacheOp::PR_NULL:
      return 0;
    case CacheOp::PR_RD_HIT:
      return 1;
    case CacheOp::PR_WR_HIT:
      return 1;
    case CacheOp::PR_RD_MISS:
    case CacheOp::PR_WR_MISS: {
      bool anyCacheContains = false;
      for (int i = 0; i < 4; i++) {
        if (caches[i]->containsAddress(address)) anyCacheContains = true;
      }
      return anyCacheContains ? 2 * blockSize / 4 : 100;
    }
  }
}

void MESIBus::transition(std::array<std::shared_ptr<Cache>, 4>&& caches, uint8_t pid, uint32_t address) {
  switch (caches[pid]->blockingOperation) {
    case CacheOp::PR_RD_HIT: {
      caches[pid]->lruShuffle(address);
      break;
    }
    case CacheOp::PR_RD_MISS: {
      bool anyCacheContains = false;
      for (int i = 0; i < 4; i++) {
        if (i != pid && caches[i]->containsAddress(address)) {
          anyCacheContains = true;
          caches[i]->updateLine(address, CacheLine::CacheState::SHARED);
        }
      }
      if (anyCacheContains) {
        caches[pid]->insertLine(address, CacheLine::CacheState::SHARED);
      } else {
        caches[pid]->insertLine(address, CacheLine::CacheState::EXCLUSIVE);
      }
    }

    case CacheOp::PR_WR_MISS:
    case CacheOp::PR_WR_HIT: {
      bool anyCacheContains = false;
      for (int i = 0; i < 4; i++) {
        if (i != pid && caches[i]->containsAddress(address)) {
          anyCacheContains = true;
          caches[i]->removeLine(address);
        }
      }
      caches[pid]->insertLine(address, CacheLine::CacheState::MODIFIED);
      break;
    }
    case CacheOp::PR_WB: {
      // SSII -> IEII
      std::vector<uint8_t> blocksToUpdate;

      for (int i = 0; i < 4; i++) {
        if (i != pid && caches[i]->containsAddress(address)) blocksToUpdate.push_back(i);
      }

      if (blocksToUpdate.size() > 1) break;
      for (auto id : blocksToUpdate) caches[id]->updateLine(address, CacheLine::CacheState::EXCLUSIVE);
    }

    case CacheOp::PR_NULL:
      break;
  }
}
}// namespace CacheSim
