#include "Cache.hpp"

#include <algorithm>

namespace CacheSim {
void Cache::lruShuffle(uint32_t address) {
  if (!has(address)) return;
  auto blockNum = address / blockSize;
  auto setNum = blockNum % numBlocks;
  auto indexInSet = getIndexOfBlockInSet(address);

  std::vector<CacheLine>& currentSet = store[setNum];

  auto line = currentSet[indexInSet];
  currentSet.erase(currentSet.begin() + indexInSet);
  currentSet.push_back(line);
}

uint32_t Cache::getIndexOfBlockInSet(uint32_t address) {
  auto blockNum = address / blockSize;
  auto setNum = blockNum % numBlocks;

  std::vector<CacheLine> currentSet = store[setNum];

  for (uint32_t i = 0; i < currentSet.size(); i++) {
    const auto& line = currentSet[i];
    if (line.blockNum == blockNum && line.state != CacheLine::CacheState::INVALID) return i;
  }
  return UINT32_MAX;
}

bool Cache::has(uint32_t address) {
  uint32_t blockNum = address / blockSize;
  uint32_t setNum = blockNum % numBlocks;

  std::vector<CacheLine> currentSet = store[setNum];

  return std::ranges::all_of(currentSet, [&blockNum](const CacheLine& line) {
    return line.blockNum == blockNum && line.state != CacheLine::CacheState::INVALID;
  });
}

void Cache::block(uint32_t address, uint32_t blockedCycles, CacheOp blockOp) {
  blockedOnAddress = address;
  blockedFor = needsWriteback ? blockedCycles + 100 : blockedCycles;
  blockingOperation = blockOp;
}

void Cache::refresh() {
  if (blockedFor > 0) --blockedFor;
}

bool Cache::needsEviction(uint32_t dirtyAddress) {
  uint32_t setNum = dirtyAddress / blockSize;
  return store[setNum][0].state != CacheLine::CacheState::INVALID;
}

void Cache::evictFor(uint32_t incomingAddress) {
  uint32_t setNum = incomingAddress / blockSize;
  if (store[setNum][0].dirty) {
    block(store[setNum][0].blockNum, 100, CacheOp::PR_WB);
  } else {
    block(store[setNum][0].blockNum, 0, CacheOp::PR_WB);
  }
}
}// namespace CacheSim
