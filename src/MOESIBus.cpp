
#include <algorithm>
#include <string>

#include "Bus.hpp"

namespace CacheSim {
uint32_t MOESIBus::getBlockedCycles(CacheOp cacheOp, uint32_t address, [[maybe_unused]] uint8_t drop_pid) {
  if (cacheOp != CacheOp::PR_NULL) updateDataAccessCount(address);
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
          monitor.trafficData += blockSize;
          anyCacheContainsDirtyLine = true;
        }
      }
      return anyCacheContainsDirtyLine ? 2 * blockSize / 4 : 100;
    }
    default:
      return 0;
  }
}

void MOESIBus::transition(uint8_t pid, uint32_t address) {
  printDebug(pid, address);
  switch (caches[pid]->blockingOperation) {
    case CacheOp::PR_RD_HIT: {
      // On read hit simply make the read block, the latest
      caches[pid]->lruShuffle(address);
      break;
    }
    case CacheOp::PR_RD_MISS: {
      if (doOtherCachesContain(pid, address)) {
        for (auto& cache : otherCachesContaining(pid, address)) {
          // For every other cache that contains the address in a MODIFIED block, transition their line to OWNED state
          if (cache->getState(address) == State::MODIFIED) cache->updateLine(address, State::OWNED);
          else if (cache->getState(address) == State::EXCLUSIVE) cache->updateLine(address, State::SHARED);
          // For every other cache that contains the address in a EXCLUSIVE block, transition their line to SHARED state
        }
        // If other caches contain the block with address, the current cache should insert a SHARED line
        caches[pid]->insertLine(address, State::SHARED);
      } else caches[pid]->insertLine(address, State::EXCLUSIVE);
      // If other caches do not contain the block with address, the current cache should insert an EXCLUSIVE line
      break;
    }
    case CacheOp::PR_WR_HIT: {
      for (auto& cache : otherCachesContaining(pid, address)) {
        // Update number of invalidations since these caches will be invalidated.
        monitor.numOfInvalidationsOrUpdates++;
        // Remove the line from the other cache that contain the address
        // On PR_WR, the writing cache has modification and read access to the block
        cache->removeLine(address);
      }
      // Update the line with the address to MODIFIED state.
      // Update is done due to PR_WR_HIT
      caches[pid]->updateLine(address, State::MODIFIED);
      // Make the updated line the latest
      caches[pid]->lruShuffle(address);
      break;
    }
    case CacheOp::PR_WR_MISS: {
      for (auto& cache : otherCachesContaining(pid, address)) {
        // Update number of invalidations since these caches will be invalidated.
        monitor.numOfInvalidationsOrUpdates++;

        // Remove the line from the other cache that contain the address
        // On PR_WR, the writing cache has modification and read access to the block
        cache->removeLine(address);
      }

      // Insert a MODIFIED line with the address in the writing cache
      // Insert is done due to PR_WR_MISS. This will also set the new line as the latest line
      caches[pid]->insertLine(address, State::MODIFIED);
      break;
    }
    case CacheOp::PR_WB:
      // WB are handled by handleEviction which remove the line. If this state is called, then line wasn't removed
      throw std::domain_error("No WB handled by transition" + std::to_string(address));
    case CacheOp::PR_NULL:
      break;
  }
  printDebug(pid, address);
}
void MOESIBus::handleEviction(uint8_t pid, uint32_t address) {
  auto blockNum = address / blockSize;
  // Remove the line to be evicted.
  caches[pid]->removeLineForBlock(blockNum);

  // Add index of caches containing the block to be evicted
  std::vector<uint8_t> blocksToUpdate;

  for (int i = 0; i < 4; i++) {
    if (i != pid && caches[i]->containsBlock(blockNum)) blocksToUpdate.push_back(i);
  }

  // If number of caches to be updated is greater than 1, then no need to change state. This implies caches are in SHARED state
  // or OWNED and SHARED state.
  if (blocksToUpdate.size() > 1) return;

  // If only 1 cache with block is left, then it needs to be put in EXCLUSIVE or OWNED state state
  for (auto id : blocksToUpdate) {
    if (caches[id]->getStateOfBlock(blockNum) == CacheLine::CacheState::OWNED)
      // If last containing cache has block is in OWNED state, transition it to MODIFIED
      caches[id]->updateLineForBlock(blockNum, CacheLine::CacheState::MODIFIED);

    // Else if last containing cache has block is in SHARED state, transition it to EXCLUSIVE
    else caches[id]->updateLineForBlock(blockNum, CacheLine::CacheState::EXCLUSIVE);
  }
}
}// namespace CacheSim
