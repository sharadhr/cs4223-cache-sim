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

  return std::ranges::all_of(currentSet, [&blockNum](const auto& line) {
    return line.blockNum == blockNum && line.state != CacheLine::CacheState::INVALID;
  });
}

void Cache::block(uint32_t address, uint32_t blockedForCycles, CacheOp blockOp) {
  blockedOnAddress = address;
  blockOperation = blockOp;
  blockedFor = blockedForCycles;
}

void Cache::prRd(uint32_t address) {
  if (has(address)) block(address, 1, CacheOp::PR_RD_HIT);
}

void Cache::prWr(uint32_t address) {
  if (has(address)) block(address, 1, CacheOp::PR_WR_HIT);
}

void Cache::update() {
  if (blockedFor > 0) --blockedFor;
  else isBlocked = false;
}
}// namespace CacheSim
