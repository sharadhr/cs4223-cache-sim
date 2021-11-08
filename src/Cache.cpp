#include "Cache.hpp"

#include "Bus.hpp"

namespace CacheSim {
void Cache::lruShuffle(uint32_t addr) {
  if (!has(addr)) {
    return;
  }
  uint32_t blockNum = addr / blockSize;
  int setNum = blockNum % totalSets;
  auto indexInSet = getIndexOfBlockInSet(addr);

  std::vector<CacheLine>& currentSet = store[setNum];

  auto line = currentSet[indexInSet];
  currentSet.erase(currentSet.begin() + indexInSet);
  currentSet.push_back(line);
}

uint32_t Cache::getIndexOfBlockInSet(uint32_t addr) {
  uint32_t blockNum = addr / blockSize;
  int setNum = blockNum % totalSets;

  std::vector<CacheLine> currentSet = store[setNum];

  for (int i = 0; i < currentSet.size(); i++) {
    auto line = currentSet[i];
    if (line.blockNum == blockNum && !line.isEmpty && line.state != CacheLine::CacheState::INVALID) {
      return i;
    }
  }
  return -1;
}

bool Cache::has(uint32_t addr) {
  uint32_t blockNum = addr / blockSize;
  int setNum = blockNum % totalSets;

  std::vector<CacheLine> currentSet = store[setNum];

  for (auto line : currentSet) {
    if (line.blockNum == blockNum && !line.isEmpty && line.state != CacheLine::CacheState::INVALID) {
      return true;
    }
  }
  return false;
}

void Cache::blockAfterPrRdHit(uint32_t addr) {
  this->blockedFor = 1;
  this->blockedOnAdress = addr;

  uint32_t blockNum = addr / blockSize;
  int setNum = blockNum % totalSets;

  std::vector<CacheLine> currentSet = store[setNum];

  for (auto line : currentSet) {
    if (line.blockNum == blockNum && !line.isEmpty && line.state != CacheLine::CacheState::INVALID) {
      this->newLine = line;
    }
  }
}

CacheLine Cache::createNewLine(uint32_t addr, CacheLine::CacheState state) {
  uint32_t blockNum = addr / blockSize;
  return CacheLine(blockNum, state);
}

void Cache::block(uint32_t blockedFor, uint32_t addr, CacheLine newLine) {
  this->blockedFor = blockedFor;
  this->blockedOnAdress = addr;
  this->newLine = newLine;
}

void Cache::prRd(uint32_t addr) {
  if (!has(addr)) {
    bus.handlePrdRdMiss(processorId, addr);
  } else {
    bus.handlePrRd(processorId, addr);
  }
}

void Cache::prWr(uint32_t addr) {
  if (!has(addr)) {
    bus.handlePrWr(processorId, addr);
  } else {
    bus.handlePrWrMiss(processorId, addr);
  }
}
}// namespace CacheSim
