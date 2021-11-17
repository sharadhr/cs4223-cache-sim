#include "Bus.hpp"

namespace CacheSim {
uint32_t DragonBus::getBlockedCycles(CacheOp cacheOp, uint32_t address, uint8_t drop_pid) {
  switch (cacheOp) {
    case CacheOp::PR_RD_HIT:
    case CacheOp::PR_WR_HIT:
      return 1;
    case CacheOp::PR_RD_MISS:
    case CacheOp::PR_WR_MISS:
      return doOtherCachesContain(drop_pid, address) ? 2 : 100;
    case CacheOp::PR_WB:
    case CacheOp::PR_NULL:
      return 0;
  }
}

void DragonBus::transition(uint8_t pid, uint32_t address) {
  /* printDebug(caches, pid, address); */
  auto triggeringCache = caches[pid];
  auto drop_pid = pid;

  switch (triggeringCache->blockingOperation) {
    case CacheOp::PR_RD_HIT:
      triggeringCache->lruShuffle(address);
      break;
    case CacheOp::PR_WR_HIT:
      // If any other caches contain this address...
      if (doOtherCachesContain(drop_pid, address)) {
        // Update the block we want in this cache, and set its state to SHARED_MODIFIED
        triggeringCache->updateLine(address, State::SHARED_MODIFIED);

        // And for the other caches that *do* contain the above-mentioned block, set them to SHARED, aka SHARED_CLEAN
        std::ranges::for_each(otherCachesContaining(drop_pid, address),
                              [&](auto& cachePtr) { cachePtr->updateLine(address, State::SHARED); });
      } else triggeringCache->updateLine(address, State::MODIFIED);
      // else, set the block in this cache to MODIFIED
      break;
    case CacheOp::PR_RD_MISS:
      // If any *other* caches contain the block associated with this address...
      if (doOtherCachesContain(drop_pid, address)) {
        // Insert the block we want into this cache, and set its state to SHARED
        triggeringCache->insertLine(address, State::SHARED);

        // And for the other caches that *do* contain the above-mentioned block, set those based on those blocks' states
        std::ranges::for_each(otherCachesContaining(drop_pid, address), [&](auto& cachePtr) {
          switch (cachePtr->getState(address)) {
            case State::SHARED:
            case State::INVALID:
            case State::OWNED:
              break;
            case State::MODIFIED:
            case State::SHARED_MODIFIED:
              // flush
              cachePtr->updateLine(address, State::SHARED_MODIFIED);
              break;
            case State::EXCLUSIVE:
              cachePtr->updateLine(address, State::SHARED);
          }
        });
      } else triggeringCache->insertLine(address, State::EXCLUSIVE);
      break;
    case CacheOp::PR_WR_MISS:
      // if any *other* caches contain the block associated with this address...
      if (doOtherCachesContain(drop_pid, address)) {
        // insert the block, and set its state to SHARED_MODIFIED
        triggeringCache->insertLine(address, State::SHARED_MODIFIED);

        // And for the other caches that *do* contain the above-mentioned block, change those blocks appropriately
        for (auto& cache : caches) {
          if (cache->containsAddress(address) && cache->pid != pid) cache->updateLine(address, State::SHARED);
        }
        std::ranges::for_each(otherCachesContaining(drop_pid, address),
                              [&](auto& cachePtr) { cachePtr->updateLine(address, State::SHARED); });
      } else triggeringCache->insertLine(address, State::MODIFIED);
      // if the block isn't shared, set ours to MODIFIED
      break;
    case CacheOp::PR_WB:
    case CacheOp::PR_NULL:
      break;
  }
  /* printDebug(caches, pid, address); */
}

void DragonBus::handleEviction(uint8_t pid, uint32_t address) {
  auto blockNum = address / blockSize;
  // SSII -> IEII
  caches[pid]->removeLineForBlock(blockNum);

  std::vector<uint8_t> blocksToUpdate;
  for (int i = 0; i < 4; i++)
    if (i != pid && caches[i]->containsBlock(address)) blocksToUpdate.push_back(i);

  if (blocksToUpdate.size() > 1) return;
  for (auto id : blocksToUpdate) {
    if (caches[id]->getStateOfBlock(blockNum) == State::SHARED_MODIFIED) {
      caches[id]->updateLineForBlock(blockNum, State::MODIFIED);
    } else if (caches[id]->getStateOfBlock(blockNum) == State::SHARED) {
      caches[id]->updateLineForBlock(blockNum, State::EXCLUSIVE);
    }
  }
}
}// namespace CacheSim
