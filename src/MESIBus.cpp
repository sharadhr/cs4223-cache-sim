
#include <algorithm>

#include "Bus.hpp"

namespace CacheSim {
uint32_t MESIBus::getBlockedCycles(std::array<std::shared_ptr<Cache>, 4>&& caches, CacheOp cacheOp, uint32_t address,
                                   uint8_t drop_pid) {
  if (cacheOp != CacheOp::PR_NULL) updateDataAcessCount(caches, address);

  switch (cacheOp) {
    case CacheOp::PR_WB:
    case CacheOp::PR_NULL:
      return 0;
    case CacheOp::PR_RD_HIT:
      return 1;
    case CacheOp::PR_WR_HIT: {
      return 1;
    }
    case CacheOp::PR_RD_MISS:
    case CacheOp::PR_WR_MISS: {
      return 100;
    }
  }
}

void MESIBus::transition(std::array<std::shared_ptr<Cache>, 4>&& caches, uint8_t pid, uint32_t address) {

  printDebug(caches, pid, address);

  switch (caches[pid]->blockingOperation) {
    case CacheOp::PR_NULL:
      return;
    case CacheOp::PR_RD_HIT: {
      // On read hit simply make the read block, the latest
      caches[pid]->lruShuffle(address);
      break;
    }
    case CacheOp::PR_RD_MISS: {
      // Check for other caches contain the same block
      bool anyCacheContains = false;
      for (int i = 0; i < 4; i++) {
        if (i != pid && caches[i]->containsAddress(address)) {
          // For every other cache that contains the address in a block, transition their line to SHARED state
          anyCacheContains = true;
          caches[i]->updateLine(address, CacheLine::CacheState::SHARED);
        }
      }

      if (anyCacheContains) {
        // If other caches contain the block with address, the current cache should insert a SHARED line
        caches[pid]->insertLine(address, CacheLine::CacheState::SHARED);
      } else {
        // If other caches do not contain the block with address, the current cache should insert an EXCLUSIVE line
        caches[pid]->insertLine(address, CacheLine::CacheState::EXCLUSIVE);
      }
      break;
    }
    case CacheOp::PR_WR_MISS: {
      // Loop through all caches and check if they contain the block with the address
      for (int i = 0; i < 4; i++) {
        if (i != pid && caches[i]->containsAddress(address)) {
          // Update number of invalidations since these caches will be invalidated.
          monitor.numOfInvalidationsOrUpdates++;

          // Remove the line from the other cache that contain the address
          // On PR_WR, the writing cache has modification and read access to the block
          caches[i]->removeLine(address);
        }
      }
      // Insert a MODIFIED line with the address in the writing cache
      // Insert is done due to PR_WR_MISS. This will also set the new line as the latest line
      caches[pid]->insertLine(address, CacheLine::CacheState::MODIFIED);
      break;
    }
    case CacheOp::PR_WR_HIT: {
      // Loop through all caches and check if they contain the block with the address
      for (int i = 0; i < 4; i++) {
        if (i != pid && caches[i]->containsAddress(address)) {
          // Update number of invalidations since these caches will be invalidated.
          monitor.numOfInvalidationsOrUpdates++;

          // Remove the line from the other cache that contain the address
          // On PR_WR, the writing cache has modification and read access to the block
          caches[i]->removeLine(address);
        }
      }

      // Update the line with the address to MODIFIED state.
      // Update is done due to PR_WR_HIT
      caches[pid]->updateLine(address, CacheLine::CacheState::MODIFIED);

      // Make the updated line the latest
      caches[pid]->lruShuffle(address);
      break;
    }
    case CacheOp::PR_WB: {
      // WB are handled by handleEviction which remove the line. If this state is called, then line wasn't removed
      throw std::domain_error("No WB handled by transition" + std::to_string(address));
    }
  }
  printDebug(caches, pid, address);
}
void MESIBus::handleEviction(std::array<std::shared_ptr<Cache>, 4>&& caches, uint8_t pid, uint32_t blockNum) {
  // Remove the line to be evicted.
  caches[pid]->removeLineForBlock(blockNum);

  // Add index of caches containing the block to be evicted
  std::vector<uint8_t> blocksToUpdate;
  for (int i = 0; i < 4; i++)
    if (i != pid && caches[i]->containsBlock(blockNum)) blocksToUpdate.push_back(i);

  // If number of caches to be updated is greater than 1, then no need to change state. This implies caches are in SHARED state.
  if (blocksToUpdate.size() > 1) return;

  // If only 1 cache with block is left, then it needs to be put in EXCLUSIVE state
  for (auto id : blocksToUpdate) caches[id]->updateLineForBlock(blockNum, CacheLine::CacheState::EXCLUSIVE);
}
}// namespace CacheSim
