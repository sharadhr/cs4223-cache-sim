
#include <algorithm>
#include <string>

#include "Bus.hpp"

namespace CacheSim {
uint32_t MESIBus::getBlockedCycles(CacheOp cacheOp, [[maybe_unused]] uint32_t address,
                                   [[maybe_unused]] uint8_t drop_pid) {

  if (cacheOp != CacheOp::PR_NULL) updateDataAccessCount(drop_pid, address);

  switch (cacheOp) {
    case CacheOp::PR_WB:
    case CacheOp::PR_NULL:
    default:
      return 0;
    case CacheOp::PR_RD_HIT:
    case CacheOp::PR_WR_HIT:
      return 1;
    case CacheOp::PR_RD_MISS:
    case CacheOp::PR_WR_MISS:
      return 100;
  }
}

void MESIBus::transition(uint8_t pid, uint32_t address) {
  printDebug(pid, address);
  switch (caches[pid]->blockingOperation) {
    case CacheOp::PR_NULL:
      return;
    case CacheOp::PR_RD_HIT: {
      // On read hit simply make the read block, the latest
      caches[pid]->lruShuffle(address);
      break;
    }
    case CacheOp::PR_RD_MISS: {
      if (doOtherCachesContain(pid, address)) {
        // For every other cache that contains the address in a block, transition their line to SHARED state
        for (auto& cache : otherCachesContaining(pid, address)) cache->updateLine(address, State::SHARED);
        // If other caches contain the block with address, the current cache should insert a SHARED line
        caches[pid]->insertLine(address, State::SHARED);
      } else caches[pid]->insertLine(address, State::EXCLUSIVE);
      // If other caches do not contain the block with address, the current cache should insert an EXCLUSIVE line
      break;
    }
    case CacheOp::PR_WR_MISS: {
      for (auto& cache : otherCachesContaining(pid, address)) {
        // Update number of invalidations since these caches will be invalidated.
        ++monitor.invalidationsCount;

        // Remove the line from the other cache that contain the address
        // On PR_WR, the writing cache has modification and read access to the block
        cache->removeLine(address);
      }
      // Insert a MODIFIED line with the address in the writing cache
      // Insert is done due to PR_WR_MISS. This will also set the new line as the latest line
      caches[pid]->insertLine(address, State::MODIFIED);
      break;
    }
    case CacheOp::PR_WR_HIT: {
      for (auto& cache : otherCachesContaining(pid, address)) {
        // Update number of invalidations since these caches will be invalidated.
        ++monitor.invalidationsCount;

        // Remove the line from other caches that contain the address
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
    case CacheOp::PR_WB: {
      // WB are handled by handleEviction which removes the line.
      // If this state is called, then line wasn't removed, and we have an error.
      throw std::domain_error("No WB handled by transition" + std::to_string(address));
    }
  }
  printDebug(pid, address);
}
void MESIBus::handleEviction(uint8_t pid, uint32_t address) {
  auto blockNum = address / blockSize;
  ++monitor.writesbackCount;

  // Remove the line to be evicted.
  caches[pid]->removeLineForBlock(blockNum);

  auto cachesToUpdate = otherCachesContaining(pid, address);

  // If only 1 cache with block is left, then it needs to be put in EXCLUSIVE state
  if (cachesToUpdate.size() == 1) cachesToUpdate[0]->updateLineForBlock(blockNum, State::EXCLUSIVE);
}
}// namespace CacheSim
