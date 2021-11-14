#include "Cache.hpp"

#include <assert.h>

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>

namespace CacheSim {
void Cache::lruShuffle(uint32_t address) {
  /* std::cout << "Shuffle address: " << address << std::endl; */
  assert(containsAddress(address));
  auto blockNum = address / blockSize;
  if (!containsBlock(blockNum)) return;
  auto& currentSet = setOfBlock(blockNum);
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

  return static_cast<uint8_t>(0 <= way && way < associativity ? way : UINT8_MAX);
}

bool Cache::containsAddress(uint32_t address) {
  auto blockNum = address / blockSize;
  return containsBlock(blockNum);
}

bool Cache::containsBlock(uint32_t blockNum) {
  auto currentSet = setOfBlock(blockNum);

  return std::ranges::any_of(currentSet, [&blockNum](const CacheLine& line) {
    /* std::cout << blockNum << " " << line.blockNum << " " << (int) line.state << " " */
    /*           << (int) (line.blockNum == blockNum && line.state != State::INVALID) << std::endl; */
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
  // TODO:  Check if Sm requires WB
  if (store[setIndex][0].state == State::MODIFIED
      || store[setIndex][0].state == CacheLine::CacheState::SHARED_MODIFIED) {
    setBlocked(store[setIndex][0].blockNum, CacheOp::PR_WB);
  }
}

CacheOp Cache::getCacheOpFor(const Type type, uint32_t address) {
  switch (type) {
    case Type::LD: {
      /* std::cout << "Address LD: " << address << std::endl; */
      auto result = containsAddress(address) ? CacheOp::PR_RD_HIT : CacheOp::PR_RD_MISS;
      /* std::cout << "RESULT: " << (int) result << std::endl; */
      return result;
    }
    case Type::ST:
      /* std::cout << "Address ST: " << address << std::endl; */
      return containsAddress(address) ? CacheOp::PR_WR_HIT : CacheOp::PR_WR_MISS;
    case Type::ALU:
    case Type::DONE:
      return CacheOp::PR_NULL;
    default:
      throw std::invalid_argument("Invalid instruction type value: " + std::to_string(static_cast<uint32_t>(type)));
  }
}

void Cache::insertLine(uint32_t address, State state) {
  uint32_t blockNum = address / blockSize;
  auto setIndex = setIndexFromAddress(address);

  store[setIndex].erase(store[setIndex].begin());
  store[setIndex].push_back(CacheLine(state, blockNum));
}

void Cache::updateLine(uint32_t address, State state) {
  assert(containsAddress(address));
  uint32_t blockNum = address / blockSize;
  auto setIndex = setIndexFromAddress(address);
  auto way = getBlockWay(blockNum);

  store[setIndex][way].state = state;
}
void Cache::removeLine(uint32_t address) {
  uint32_t blockNum = address / blockSize;
  auto setIndex = setIndexFromAddress(address);
  auto way = getBlockWay(blockNum);

  auto line = store[setIndex][way];

  store[setIndex].erase(store[setIndex].begin() + way);
  line.state = State::INVALID;
  store[setIndex].insert(store[setIndex].begin(), line);
}

CacheLine::CacheState Cache::getState(uint32_t address) {
  uint32_t blockNum = address / blockSize;
  auto setIndex = setIndexFromAddress(address);
  auto way = getBlockWay(blockNum);
  return way == UINT8_MAX ? State::INVALID : store[setIndex][way].state;
}

}// namespace CacheSim
