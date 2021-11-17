
#include <algorithm>

#include "Bus.hpp"

namespace CacheSim {
uint32_t MOESIBus::getBlockedCycles(std::array<std::shared_ptr<Cache>, 4>&& caches, CacheOp cacheOp, uint32_t address,
                                    uint8_t drop_pid) {
  if (cacheOp != CacheOp::PR_NULL) updateDataAcessCount(caches, address);
  switch (cacheOp) {
    case CacheOp::PR_WB:
    case CacheOp::PR_NULL:
      return 0;
    case CacheOp::PR_RD_HIT: {
      return 1;
    }
    case CacheOp::PR_WR_HIT: {
      return 1;
    }
    case CacheOp::PR_RD_MISS:
    case CacheOp::PR_WR_MISS: {
      bool anyCacheContainsDirtyLine = false;
      for (int i = 0; i < 4; i++) {
        if (caches[i]->containsAddress(address)
            && (caches[i]->getState(address) == State::OWNED || caches[i]->getState(address) == State::MODIFIED)) {
          monitor.trafficData += blockSize;
          anyCacheContainsDirtyLine = true;
        }
      }
      return anyCacheContainsDirtyLine ? 2 * blockSize / 4 : 100;
    }
  }
}

void MOESIBus::transition(std::array<std::shared_ptr<Cache>, 4>&& caches, uint8_t pid, uint32_t address) {
  printDebug(caches, pid, address);
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
          if (caches[i]->getState(address) == CacheLine::CacheState::MODIFIED) {
            caches[i]->updateLine(address, CacheLine::CacheState::OWNED);
          } else if (caches[i]->getState(address) == CacheLine::CacheState::EXCLUSIVE) {
            caches[i]->updateLine(address, CacheLine::CacheState::SHARED);
          }
        }
      }
      if (anyCacheContains) {
        caches[pid]->insertLine(address, CacheLine::CacheState::SHARED);
      } else {
        caches[pid]->insertLine(address, CacheLine::CacheState::EXCLUSIVE);
      }
      break;
    }
    case CacheOp::PR_WR_HIT: {
      for (int i = 0; i < 4; i++) {
        if (i != pid && caches[i]->containsAddress(address)) {
          monitor.numOfInvalidationsOrUpdates++;
          caches[i]->removeLine(address);
        }
      }
      caches[pid]->updateLine(address, CacheLine::CacheState::MODIFIED);
      caches[pid]->lruShuffle(address);
      break;
    }
    case CacheOp::PR_WR_MISS: {
      for (int i = 0; i < 4; i++) {
        if (i != pid && caches[i]->containsAddress(address)) {
          monitor.numOfInvalidationsOrUpdates++;
          caches[i]->removeLine(address);
        }
      }
      caches[pid]->insertLine(address, CacheLine::CacheState::MODIFIED);
      break;
    }
    case CacheOp::PR_WB: {
      throw std::domain_error("No WB handled by transition" + std::to_string(address));
    }
    case CacheOp::PR_NULL:
      break;
  }
  printDebug(caches, pid, address);
}
void MOESIBus::handleEviction(std::array<std::shared_ptr<Cache>, 4>&& caches, uint8_t pid, uint32_t blockNum) {
  // SOII -> IMII
  // SSII -> IEII

  caches[pid]->removeLineForBlock(blockNum);

  std::vector<uint8_t> blocksToUpdate;

  for (int i = 0; i < 4; i++) {
    if (i != pid && caches[i]->containsBlock(blockNum)) blocksToUpdate.push_back(i);
  }

  if (blocksToUpdate.size() > 1) return;

  for (auto id : blocksToUpdate) {
    if (caches[id]->getStateOfBlock(blockNum) == CacheLine::CacheState::OWNED)
      caches[id]->updateLineForBlock(blockNum, CacheLine::CacheState::MODIFIED);
    else caches[id]->updateLineForBlock(blockNum, CacheLine::CacheState::EXCLUSIVE);
  }
}
}// namespace CacheSim
