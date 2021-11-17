#include "Cache.hpp"

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <string>

namespace CacheSim {
void Cache::lruShuffle(uint32_t address) {
  auto blockNum = address / blockSize;
  if (!containsBlockSus(blockNum))
    throw std::domain_error("Shuffle on nonexistent address: " + std::to_string(address));
  auto& currentSet = setOfBlock(blockNum);
  auto way = getBlockWaySus(blockNum);

  auto line = currentSet[way];
  currentSet.erase(currentSet.begin() + way);
  currentSet.push_back(line);
}

uint8_t Cache::getBlockWay(uint32_t blockNum) {
  auto currentSet = setOfBlock(blockNum);

  auto way = std::distance(currentSet.begin(),
                           std::find_if(currentSet.begin(), currentSet.end(), [&blockNum](const CacheLine& line) {
                             return line.state != State::INVALID && line.blockNum == blockNum;
                           }));

  return static_cast<uint8_t>(0 <= way && way < associativity ? way : UINT8_MAX);
}

uint8_t Cache::getBlockWaySus(uint32_t blockNum) {
  auto currentSet = setOfBlock(blockNum);

  auto way = std::distance(currentSet.begin(),
                           std::find_if(currentSet.begin(), currentSet.end(),
                                        [&blockNum](const CacheLine& line) { return line.blockNum == blockNum; }));

  return static_cast<uint8_t>(0 <= way && way < associativity ? way : UINT8_MAX);
}

bool Cache::containsAddress(uint32_t address) {
  auto blockNum = address / blockSize;
  return containsBlock(blockNum);
}

bool Cache::containsBlock(uint32_t blockNum) {
  auto currentSet = setOfBlock(blockNum);

  return std::ranges::any_of(currentSet, [&blockNum](const CacheLine& line) {
    return line.blockNum == blockNum && line.state != State::INVALID;
  });
}

bool Cache::containsBlockSus(uint32_t blockNum) { return getBlockWaySus(blockNum) != UINT8_MAX; }

bool Cache::needsEvictionFor(uint32_t incomingAddress) {
  auto setIndex = setIndexFromAddress(incomingAddress);
  return store[setIndex][0].state != State::INVALID;
}

bool Cache::needsWriteBack(uint32_t incomingAddress) {
  uint32_t setIndex = setIndexFromAddress(incomingAddress);
  return store[setIndex][0].state == State::MODIFIED || store[setIndex][0].state == State::SHARED_MODIFIED
      || store[setIndex][0].state == State::OWNED;
}

uint32_t Cache::evictedBlockFor(uint32_t incomingAddress) {
  uint32_t setIndex = setIndexFromAddress(incomingAddress);
  return store[setIndex][0].blockNum;
}

void Cache::setCacheOpFor(const Type& type, uint32_t address) {
  blockingCacheBlock = address / blockSize;
  switch (type) {
    case Type::LD: {
      blockingOperation = containsAddress(address) ? CacheOp::PR_RD_HIT : CacheOp::PR_RD_MISS;
      break;
    }
    case Type::ST: {
      blockingOperation = containsAddress(address) ? CacheOp::PR_WR_HIT : CacheOp::PR_WR_MISS;
      break;
    }
    case Type::ALU:
    case Type::DONE: {
      blockingOperation = CacheOp::PR_NULL;
      break;
    }
    default:
      throw std::invalid_argument("Invalid instruction type value: " + std::to_string(static_cast<uint32_t>(type)));
  }
}

void Cache::insertLine(uint32_t address, State state) {
  auto blockNum = address / blockSize;
  auto setIndex = setIndexFromAddress(address);

  if (store[setIndex][0].state != State::INVALID)
    throw std::domain_error("Eviction didnt happen: " + std::to_string(address));

  store[setIndex].erase(store[setIndex].begin());
  store[setIndex].push_back(CacheLine(state, blockNum));

  if (store[setIndex].size() != associativity)
    throw std::domain_error("InsertLine broke set: " + std::to_string(setIndex));
}

void Cache::updateLine(uint32_t address, State state) {
  auto blockNum = address / blockSize;
  updateLineForBlock(blockNum, state);
}

void Cache::updateLineForBlock(uint32_t blockNum, State state) {
  if (!containsBlockSus(blockNum))
    throw std::domain_error("Update on nonexistent blockNum: " + std::to_string(blockNum));
  auto setIndex = blockNum % numSets;
  auto way = getBlockWaySus(blockNum);

  store[setIndex][way].state = state;

  if (store[setIndex].size() != associativity)
    throw std::domain_error("UpdateLine broke set: " + std::to_string(setIndex));
}

void Cache::removeLine(uint32_t address) {
  uint32_t blockNum = address / blockSize;
  removeLineForBlock(blockNum);
}

void Cache::removeLineForBlock(uint32_t blockNum) {
  if (!containsBlockSus(blockNum))
    throw std::domain_error("Update on nonexistent blockNum: " + std::to_string(blockNum));

  auto setIndex = blockNum % numSets;
  auto way = getBlockWaySus(blockNum);

  auto line = store[setIndex][way];

  store[setIndex].erase(store[setIndex].begin() + way);
  line.state = State::INVALID;
  store[setIndex].insert(store[setIndex].begin(), line);

  if (store[setIndex].size() != associativity)
    throw std::domain_error("RemoveLine broke set: " + std::to_string(setIndex));
}

State Cache::getState(uint32_t address) {
  auto blockNum = address / blockSize;
  return getStateOfBlock(blockNum);
}

State Cache::getStateOfBlock(uint32_t blockNum) {
  auto setIndex = blockNum % numSets;
  auto way = getBlockWay(blockNum);
  return way == UINT8_MAX ? State::INVALID : store[setIndex][way].state;
}

}// namespace CacheSim
