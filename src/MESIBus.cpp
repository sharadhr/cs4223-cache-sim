#include <cassert>
#include <string>
#include <utility>
#include <vector>

#include "Bus.hpp"

using namespace std;

namespace CacheSim {
bool MESIBus::cacheAllocAddr(int processorId, int addr, CacheLine::CacheState addrState) {
  Cache& cache = processors->at(processorId).cache;
  int blockNum = addr / blockSize;
  int availableTime = getMemBlockAvailableTime(blockNum) + 100;
  CacheLine evictedEntry = cache.evict(addr);
  if (evictedEntry.state != CacheLine::INVALID) {
    if (evictedEntry.state == CacheLine::DIRTY) {
      int evictedAddr = cache.getHeadAddr(evictedEntry);
      writeBackMem(processorId, evictedAddr);
    }
  }

  cache.lruShuffle(addr);
  cache.put(addr, addrState, availableTime);
  return true;
}

void MESIBus::invalidate(int processorId, int addr, bool needWriteBack) {
  for (int othCacheID = 0; othCacheID < (int) processors->size(); othCacheID++) {
    if (othCacheID == processorId) continue;
    Cache& othCache = processors->at(othCacheID).cache;

    if (othCache.has(addr)) {
      if (othCache.isAddrDirty(addr) && needWriteBack) {
        writeBackMem(othCacheID, addr);
      }
      othCache.setBlockState(addr, CacheLine::INVALID);
      monitor.invalidateCount++;
    }
  }
}

void MESIBus::readHit(int coreID, int addr) {
  Cache& cache = processors->at(coreID).cache;
  cache.lruShuffle(addr);
}

void MESIBus::writeHit(int coreID, int addr) {
  int processorId = coreID;
  Cache& cache = processors->at(coreID).cache;

  cache.lruShuffle(addr);
  cache.setBlockState(addr, CacheLine::DIRTY);

  invalidate(processorId, addr, false);

  int blockNum = addr / blockSize;
  invalidBlock[blockNum] = -2;
}

void MESIBus::readMiss(int coreID, int addr) {
  int processorId = coreID;

  for (int othCacheID = 0; othCacheID < (int) processors->size(); othCacheID++) {
    if (othCacheID == processorId) continue;

    Cache& othCache = processors->at(othCacheID).cache;
    if (othCache.has(addr)) {
      if (othCache.isAddrDirty(addr)) {
        writeBackMem(othCacheID, addr);
        othCache.setBlockState(addr, CacheLine::SHARED);
      }
    }
  }

  int countHold = 0;
  for (int othCacheID = 0; othCacheID < (int) processors->size(); othCacheID++) {
    if (othCacheID == processorId) continue;
    Cache& othCache = processors->at(othCacheID).cache;
    if (othCache.has(addr)) {
      countHold++;
    }
  }

  auto state = (countHold == 0) ? CacheLine::EXCLUSIVE : CacheLine::SHARED;
  cacheAllocAddr(processorId, addr, state);

  monitor.trafficData += blockSize;
}

void MESIBus::writeMiss(int coreID, int addr) {
  int processorId = coreID;
  invalidate(processorId, addr, true);
  cacheAllocAddr(processorId, addr, CacheLine::DIRTY);
  int blockNum = addr / blockSize;
  invalidBlock[blockNum] = -2;

  monitor.trafficData += blockSize;
}

MESIBus::~MESIBus() {
  delete (processors);
}
}// namespace CacheSim
