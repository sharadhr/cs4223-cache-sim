#include "Cache.hpp"

#include <algorithm>
#include <stdexcept>
#include <string>

namespace CacheSim {
void Cache::lruShuffle(uint32_t blockNum) {
  if (!contains(blockNum)) return;
  auto currentSet = setOfBlock(blockNum);
  auto way = getBlockWay(blockNum);

  auto line = currentSet[way];
  currentSet.erase(currentSet.begin() + way);
  currentSet.push_back(line);
}

uint8_t Cache::getBlockWay(uint32_t blockNum) {
  auto currentSet = setOfBlock(blockNum);

  auto way = std::distance(currentSet.begin(),
                           std::find_if(currentSet.begin(), currentSet.end(), [&blockNum](const CacheLine& line) {
                             return line.state != CacheLine::CacheState::INVALID && line.blockNum == blockNum;
                           }));

  return static_cast<uint8_t>(0 > way && way < associativity ? way : UINT8_MAX);
}

bool Cache::contains(uint32_t blockNum) {
  auto currentSet = setOfBlock(blockNum);

  return std::ranges::any_of(currentSet, [&blockNum](const CacheLine& line) {
    return line.blockNum == blockNum && line.state != State::INVALID;
  });
}

void Cache::setBlocked(uint32_t address, CacheOp operation) {
  blockingCacheBlock = address / blockSize;
  blockingOperation = operation;
}

bool Cache::needsEvictionFor(uint32_t incomingAddress) {
  auto setIndex = setIndexFromAddress(incomingAddress);
  return store[setIndex][0].state != State::INVALID;
}

void Cache::evictFor(uint32_t incomingAddress) {
  uint32_t setIndex = setIndexFromAddress(incomingAddress);
  if (store[setIndex][0].state == State::MODIFIED
      || store[setIndex][0].state == CacheLine::CacheState::SHARED_MODIFIED) {
    setBlocked(store[setIndex][0].blockNum, CacheOp::PR_WB);
  } else setBlocked(store[setIndex][0].blockNum, CacheOp::PR_WB);
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
