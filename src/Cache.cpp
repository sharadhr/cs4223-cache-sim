#include "Cache.hpp"

#include <algorithm>
#include <stdexcept>
#include <string>

namespace CacheSim {
void Cache::lruShuffle(uint32_t blockNum) {
  if (!contains(blockNum)) return;
  auto setIndex = blockNum % numBlocks;
  auto currentSet = store[setIndex];
  auto way = getBlockWay(blockNum);

  auto line = currentSet[way];
  currentSet.erase(currentSet.begin() + way);
  currentSet.push_back(line);
}

uint8_t Cache::getBlockWay(uint32_t blockNum) {
  auto setIndex = setIndexFromBlock(blockNum);
  auto currentSet = store[setIndex];

  for (uint8_t i = 0; i < currentSet.size(); i++) {
    const auto& line = currentSet[i];
    if (line.blockNum == blockNum && line.state != State::INVALID) return i;
  }
  return UINT8_MAX;
}

bool Cache::contains(uint32_t blockNum) {
  auto setIndex = setIndexFromBlock(blockNum);
  auto currentSet = store[setIndex];

  return std::ranges::any_of(currentSet, [&blockNum](const CacheLine& line) {
    return line.blockNum == blockNum && line.state != State::INVALID;
  });
}

void Cache::setBlocked(uint32_t address, uint32_t blockedCycles, CacheOp operation) {
  blockedOnBlock = address / blockSize;
  blockedFor = blockedCycles;
  blockingOperation = operation;
}

void Cache::refresh() {
  if (blockedFor > 0) --blockedFor;
}

bool Cache::needsEvictionFor(uint32_t incomingAddress) {
  auto setIndex = setIndexFromAddress(incomingAddress);
  return store[setIndex][0].state != State::INVALID;
}

void Cache::evictFor(uint32_t incomingAddress) {
  uint32_t setIndex = setIndexFromAddress(incomingAddress);
  if (store[setIndex][0].state == State::MODIFIED
      || store[setIndex][0].state == CacheLine::CacheState::SHARED_MODIFIED) {
    setBlocked(store[setIndex][0].blockNum, 100, CacheOp::PR_WB);
  } else setBlocked(store[setIndex][0].blockNum, 0, CacheOp::PR_WB);
}

CacheOp Cache::getCacheOpFor(const Type& type, uint32_t address) {
  switch (type) {
    case Type::LD:
      return contains(address) ? CacheOp::PR_RD_HIT : CacheOp::PR_RD_MISS;
    case Type::ST:
      return contains(address) ? CacheOp::PR_WR_HIT : CacheOp::PR_WR_MISS;
    case Type::ALU:
    case Type::DONE:
      return CacheOp::PR_NULL;
    default:
      throw std::invalid_argument("Invalid instruction type value: " + std::to_string(static_cast<uint32_t>(type)));
  }
}
}// namespace CacheSim
