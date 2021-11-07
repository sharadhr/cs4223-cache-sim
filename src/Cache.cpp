#include "Cache.hpp"

#include <cassert>
#include <string>
#include <utility>
#include <vector>

using namespace std;

namespace CacheSim {
void Cache::lruShuffle(uint32_t setNum, uint32_t indexInSet) {
  vector<CacheLine>& currentSet = store[setNum];
  CacheLine line = currentSet[indexInSet];
  currentSet.erase(currentSet.begin() + indexInSet);
  currentSet.push_back(line);
}

void Cache::lruShuffle(uint32_t addr) {
  int blockNumber = addr / blockSize;
  int setNum = blockNumber % totalSets;
  int indexInSet = getIndexInSet(setNum);
  vector<CacheLine>& currentSet = store[setNum];
  CacheLine line = currentSet[indexInSet];
  currentSet.erase(currentSet.begin() + indexInSet);
  currentSet.push_back(line);
}

int Cache::getInvalidLineFromSet(uint32_t setNum) {
  std::vector<CacheLine>& currentSet = store[setNum];
  for (int num = 0; num < associativity; num++) {
    CacheLine line = currentSet[num];
    if (line.state == CacheLine::CacheState::INVALID) {
      return num;
    }
  }
  return -1;
}

int Cache::getEvictionCandidateFromSet(uint32_t setNum) {
  int invalidIndex = getInvalidLineFromSet(setNum);
  if (invalidIndex == -1) {
    return invalidIndex;
  }
  return 0;
}

CacheLine& Cache::evict(uint32_t addr) {
  assert(!has(addr));
  int blockNumber = addr / blockSize;
  int setNum = blockNumber % totalSets;
  int lineToEvect = getEvictionCandidateFromSet(setNum);
  store[setNum][lineToEvect] = CacheLine();
  return store[setNum][lineToEvect];
}

int Cache::getIndexInSet(uint32_t addr) {
  int blockNumber = addr / blockSize;
  int setNum = blockNumber % totalSets;
  std::vector<CacheLine>& currentSet = store[setNum];

  for (int num = 0; num < associativity; num++) {
    CacheLine line = currentSet[num];
    if (line.state != CacheLine::CacheState::INVALID && line.blockNumber == blockNumber) {
      return num;
    }
  }

  return -1;
}

bool Cache::isAddrDirty(uint32_t addr) {
  int blockNumber = addr / blockSize;
  int setNum = blockNumber % totalSets;
  return store[setNum][getIndexInSet(addr)].state == CacheLine::DIRTY;
}

bool Cache::setBlockState(uint32_t addr, CacheLine::CacheState state) {
  int blockNumber = addr / blockSize;
  int setNum = blockNumber % totalSets;
  return store[setNum][getIndexInSet(addr)].state == state;
}

CacheLine::CacheState Cache::getBlockState(uint32_t addr) {
  int blockNumber = addr / blockSize;
  int setNum = blockNumber % totalSets;
  return store[setNum][getIndexInSet(addr)].state;
}

int Cache::getHeadAddr(CacheLine line) {
  int blockNumber = line.blockNumber;
  return blockNumber * blockSize;
}

bool Cache::has(uint32_t addr) {
  return getIndexInSet(addr) != -1;
}

CacheLine& Cache::get(uint32_t addr) {
  assert(has(addr));
  int blockNumber = addr / blockSize;
  int setNum = blockNumber % totalSets;
  int indexInSet = getIndexInSet(addr);
  return store[setNum][indexInSet];
}

void Cache::put(uint32_t addr, CacheLine::CacheState state, int validFrom) {
  assert(!has(addr));
  int blockNumber = addr / blockSize;
  int setNum = blockNumber % totalSets;
  int lineToEvect = getEvictionCandidateFromSet(setNum);
  store[setNum][lineToEvect] = CacheLine(state, blockNumber, validFrom);
}
}// namespace CacheSim
