
#include <algorithm>
#include <string>

#include "Bus.hpp"

namespace CacheSim {
uint32_t MESIBus::getBlockedCycles(CacheOp cacheOp, [[maybe_unused]] uint32_t address,
                                   [[maybe_unused]] uint8_t drop_pid) {
  switch (cacheOp) {
    case CacheOp::PR_WB:
      // monitor.trafficBytes += blockSize;
    case CacheOp::PR_NULL:
      return 0;
    case CacheOp::PR_RD_HIT:
      return 1;
    case CacheOp::PR_WR_HIT: {
      // monitor.trafficBytes += blockSize;
      return 1;
    }
    case CacheOp::PR_RD_MISS:
    case CacheOp::PR_WR_MISS: {
      // monitor.trafficBytes += blockSize;
      return 100;
    }
    default:
      return 0;
  }
}

void MESIBus::transition(uint8_t pid, uint32_t address) {

  printDebug(pid, address);

  switch (caches[pid]->blockingOperation) {
    case CacheOp::PR_NULL:
      return;
    case CacheOp::PR_RD_HIT: {
      caches[pid]->lruShuffle(address);
      break;
    }
    case CacheOp::PR_RD_MISS: {

      bool anyCacheContains = false;
      for (int i = 0; i < 4; i++) {
        if (i != pid && caches[i]->containsAddress(address)) {
          anyCacheContains = true;
          caches[i]->updateLine(address, State::SHARED);
        }
      }
      if (anyCacheContains) {
        caches[pid]->insertLine(address, State::SHARED);
      } else {
        caches[pid]->insertLine(address, State::EXCLUSIVE);
      }
      break;
    }
    case CacheOp::PR_WR_MISS: {
      for (int i = 0; i < 4; i++) {
        if (i != pid && caches[i]->containsAddress(address)) {
          // monitor.numOfInvalidationsOrUpdates++;
          caches[i]->removeLine(address);
        }
      }
      caches[pid]->insertLine(address, State::MODIFIED);
      break;
    }
    case CacheOp::PR_WR_HIT: {
      for (int i = 0; i < 4; i++) {
        if (i != pid && caches[i]->containsAddress(address)) {
          // monitor.numOfInvalidationsOrUpdates++;
          caches[i]->removeLine(address);
        }
      }
      caches[pid]->updateLine(address, State::MODIFIED);
      caches[pid]->lruShuffle(address);
      break;
    }
    case CacheOp::PR_WB: {
      throw std::domain_error("No WB handled by transition" + std::to_string(address));
    }
  }
  printDebug(pid, address);
}
void MESIBus::handleEviction(uint8_t pid, uint32_t address) {
  auto blockNum = address / blockSize;
  // SSII -> IEII
  caches[pid]->removeLineForBlock(blockNum);

  std::vector<uint8_t> blocksToUpdate;
  for (int i = 0; i < 4; i++)
    if (i != pid && caches[i]->containsBlock(blockNum)) blocksToUpdate.push_back(i);

  if (blocksToUpdate.size() > 1) return;
  for (auto id : blocksToUpdate) caches[id]->updateLineForBlock(blockNum, State::EXCLUSIVE);
}
}// namespace CacheSim
