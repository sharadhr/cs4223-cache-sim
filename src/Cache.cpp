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

void Cache::block(uint32_t addr, uint32_t blockedFor, CacheOp blockOp) {
  this->blockedFor = 1;
  this->blockedOnAdress = addr;
  this->blockOperation = blockOp;
}

CacheLine Cache::createNewLine(uint32_t addr, CacheLine::CacheState state) {
  uint32_t blockNum = addr / blockSize;
  return CacheLine(blockNum, state);
}

void Cache::prRd(uint32_t addr) {
  if (has(addr)) {
    block(addr, 1, CacheOp::PR_RD_HIT);
  } else {
    bus.handlePrdRdMiss(processorId, addr);
  }
}

void Cache::prWr(uint32_t addr) {
  if (has(addr)) {
    block(addr, 1, CacheOp::PR_WR_HIT);
  } else {
    bus.handlePrWrMiss(processorId, addr);
  }
}

void Cache::update() {
  if (blockedFor > 0) {
    blockedFor--;
  } else {
    issueBusTransaction();
    isBlocked = false;
  }
}
}// namespace CacheSim
