
#include <algorithm>
#include <string>

#include "Bus.hpp"

namespace CacheSim {
uint32_t MOESIBus::getBlockedCycles(CacheOp cacheOp, uint32_t address, [[maybe_unused]] uint8_t drop_pid) {
  switch (cacheOp) {
    case CacheOp::PR_WB:
    case CacheOp::PR_NULL:
      return 0;
    case CacheOp::PR_RD_HIT:
    case CacheOp::PR_WR_HIT:
      return 1;
    case CacheOp::PR_RD_MISS:
    case CacheOp::PR_WR_MISS: {
      bool anyCacheContainsDirtyLine = false;
      for (int i = 0; i < 4; i++) {
        if (caches[i]->containsAddress(address)
            && (caches[i]->getState(address) == State::OWNED || caches[i]->getState(address) == State::MODIFIED)) {
          anyCacheContainsDirtyLine = true;
        }
      }
      return anyCacheContainsDirtyLine ? 2 * blockSize / 4 : 100;
    }
  }
}

void MOESIBus::transition(uint8_t pid, uint32_t address) {
  printDebug(pid, address);
  switch (caches[pid]->blockingOperation) {
    case CacheOp::PR_RD_HIT: {
      caches[pid]->lruShuffle(address);
      break;
    }
    case CacheOp::PR_RD_MISS: {
      if (doOtherCachesContain(pid, address)) {
        for (auto& cache : otherCachesContaining(pid, address)) {
          if (cache->getState(address) == State::MODIFIED) cache->updateLine(address, State::OWNED);
          else if (cache->getState(address) == State::EXCLUSIVE) cache->updateLine(address, State::SHARED);
        }
        caches[pid]->insertLine(address, State::SHARED);
      } else caches[pid]->insertLine(address, State::EXCLUSIVE);
      break;
    }
    case CacheOp::PR_WR_HIT: {
      for (auto& cache : otherCachesContaining(pid, address)) cache->removeLine(address);
      caches[pid]->updateLine(address, State::MODIFIED);
      caches[pid]->lruShuffle(address);
      break;
    }
    case CacheOp::PR_WR_MISS: {
      for (auto& cache : otherCachesContaining(pid, address)) cache->removeLine(address);
      caches[pid]->insertLine(address, State::MODIFIED);
      break;
    }
    case CacheOp::PR_WB:
      throw std::domain_error("No WB handled by transition" + std::to_string(address));
    case CacheOp::PR_NULL:
      break;
  }
  printDebug(pid, address);
}
void MOESIBus::handleEviction(uint8_t pid, uint32_t address) {
  // SOII -> IMII
  // SSII -> IEII

  caches[pid]->removeLineForBlock(address);

  for (auto& cache : otherCachesContaining(pid, address)) {
    if (cache->getStateOfBlock(address) == State::OWNED) cache->updateLineForBlock(address, State::MODIFIED);
    else cache->updateLineForBlock(address, State::EXCLUSIVE);
  }
}
}// namespace CacheSim
