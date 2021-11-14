#include "Bus.hpp"

namespace CacheSim {
uint32_t DragonBus::getBlockedCycles(std::array<std::shared_ptr<Cache>, 4>&& caches, CacheOp cacheOp, uint32_t address,
                                     uint8_t drop_pid) {
  switch (cacheOp) {
    case CacheOp::PR_RD_HIT:
    case CacheOp::PR_WR_HIT:
      return 1;
    case CacheOp::PR_RD_MISS:
    case CacheOp::PR_WR_MISS:
      return doOtherCachesContain(caches, drop_pid, address) ? 2 : 100;
    case CacheOp::PR_WB:
    case CacheOp::PR_NULL:
      return 0;
  }
}

void DragonBus::transition(std::array<std::shared_ptr<Cache>, 4>&& caches, uint8_t pid, uint32_t address) {
  auto triggeringCache = caches[pid];
  auto drop_pid = pid;

  switch (triggeringCache->blockingOperation) {
    case CacheOp::PR_RD_HIT:
      triggeringCache->lruShuffle(address);
    case CacheOp::PR_WR_HIT:
      // If any other caches contain this address...
      if (doOtherCachesContain(caches, drop_pid, address)) {
        // Update the block we want in this cache, and set its state to SHARED_MODIFIED
        triggeringCache->updateLine(address, State::SHARED_MODIFIED);

        // And for the other caches that *do* contain the above-mentioned block, set them to SHARED, aka SHARED_CLEAN
        std::ranges::for_each(otherCachesContaining(caches, drop_pid, address),
                              [&](auto& cachePtr) { cachePtr->updateLine(address, State::SHARED); });
      } else triggeringCache->updateLine(address, State::MODIFIED);
      // else, set the block in this cache to MODIFIED
      return;
    case CacheOp::PR_RD_MISS:
      // If any *other* caches contain the block associated with this address...
      if (doOtherCachesContain(caches, drop_pid, address)) {
        // Insert the block we want into this cache, and set its state to SHARED
        triggeringCache->insertLine(address, State::SHARED);

        // And for the other caches that *do* contain the above-mentioned block, set those based on those blocks' states
        std::ranges::for_each(otherCachesContaining(caches, drop_pid, address), [&](std::shared_ptr<Cache>& cachePtr) {
          switch (cachePtr->getState(address)) {
            case State::SHARED:
            case State::INVALID:
              return;
            case State::MODIFIED:
            case State::SHARED_MODIFIED:
              // flush
              cachePtr->updateLine(address, State::SHARED_MODIFIED);
              return;
            case State::EXCLUSIVE:
              cachePtr->updateLine(address, State::SHARED);
          }
        });
      } else triggeringCache->insertLine(address, State::EXCLUSIVE);
      return;
    case CacheOp::PR_WR_MISS:
      // if any *other* caches contain the block associated with this address...
      if (doOtherCachesContain(caches, drop_pid, address)) {
        // insert the block, and set its state to SHARED_MODIFIED
        triggeringCache->insertLine(address, State::SHARED_MODIFIED);

        // And for the other caches that *do* contain the above-mentioned block, set those blocks appropriately
        std::ranges::for_each(otherCachesContaining(caches, drop_pid, address), [&](std::shared_ptr<Cache>& cachePtr) {
          switch (cachePtr->getState(address)) {
            case State::SHARED_MODIFIED:
            case State::MODIFIED:
            case State::EXCLUSIVE:
              cachePtr->updateLine(address, State::SHARED);
              return;
            case State::SHARED:
            case State::INVALID:
              return;
          }
        });
      } else triggeringCache->insertLine(address, State::MODIFIED);
      // if the block isn't shared, set ours to MODIFIED
      return;
    case CacheOp::PR_WB:
    case CacheOp::PR_NULL:
      return;
  }
}
}// namespace CacheSim
