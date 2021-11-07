#include <cassert>
#include <string>
#include <utility>
#include <vector>

#include "Bus.hpp"

using namespace std;

namespace CacheSim {
int DragonBus::findCacheSourceAvailableTime(int cacheID, int addr) {
  int availableTimeFromOth = -2;

  for (int othCacheID = 0; othCacheID < (int) processors.size(); othCacheID++) {
    if (othCacheID == cacheID) continue;
    Cache& othCache = processors[othCacheID].cache;
    if (othCache.has(addr)) {
      availableTimeFromOth = min(availableTimeFromOth,
                                 max(othCache.get(addr).validFrom, curTime));
    }
  }
  return availableTimeFromOth;
}

int DragonBus::findMemSourceAvailableTime(int addr) {
  int blockNum = addr / blockSize;

  int availableTimeFromMem = getMemBlockAvailableTime(blockNum);
  return availableTimeFromMem;
}

int DragonBus::findSourceAvailableTime(int cacheID, int addr) {
  int availableTimeFromMem = findMemSourceAvailableTime(addr);
  int availableTimeFromCache = findCacheSourceAvailableTime(cacheID, addr);
  return min(availableTimeFromMem + 100,
             availableTimeFromCache + 2 * blockSize / 4);
}

int DragonBus::countOthCacheHold(int cacheID, int addr) {
  int countHold = 0;
  for (int othCacheID = 0; othCacheID < (int) processors.size(); othCacheID++) {
    if (othCacheID == cacheID) continue;
    Cache& othCache = processors[othCacheID].cache;
    if (othCache.has(addr)) {
      countHold++;
    }
  }
  return countHold;
}

void DragonBus::cacheReceiveW(int cacheID, int addr, int sendCycle) {
  Cache& cache = processors[cacheID].cache;
  assert(cache.has(addr));

  cache.get(addr).validFrom = sendCycle + 2;

  monitor.trafficData += 4;
}

void DragonBus::cacheReceiveB(int cacheID, int addr, CacheLine::CacheState state) {
  Cache& cache = processors[cacheID].cache;
  assert(!cache.has(addr));

  int cacheAvailableTime = max(findCacheSourceAvailableTime(cacheID, addr), curTime);
  int memAvailableTime = max(findMemSourceAvailableTime(addr), curTime);

  int availableTime = cacheAvailableTime == -2
      ? memAvailableTime + 100
      : cacheAvailableTime + 2 * blockSize / 4;

  CacheLine& evictedEntry = cache.evict(addr);
  if (evictedEntry.state != CacheLine::INVALID) {
    int evictedAddr = cache.getHeadAddr(evictedEntry);
    int evictedBlockNum = evictedAddr / blockSize;
    bool needRewrite = (getMemBlockAvailableTime(evictedBlockNum) == -2
                        && countOthCacheHold(cacheID, evictedAddr) == 0);

    if (needRewrite) {
      writeBackMem(cacheID, evictedAddr);
    }
  }

  cache.lruShuffle(addr);
  cache.put(addr, state, availableTime);

  monitor.invalidateCount += blockSize;
}

void DragonBus::broadcastWOthCache(int cacheID, int addr, int sendCycle) {
  int countHold = countOthCacheHold(cacheID, addr);
  int headAddr = getHeadAddr(addr);
  assert(countHold > 0);
  broadcastingBlocks[headAddr] = sendCycle + 2;

  for (int othCacheID = 0; othCacheID < (int) processors.size(); othCacheID++) {
    if (othCacheID == cacheID) continue;
    Cache& othCache = processors[othCacheID].cache;
    if (othCache.has(addr)) {
      cacheReceiveW(othCacheID, addr, sendCycle);
      othCache.setBlockState(addr, CacheLine::SHARED);

      monitor.updateCount++;
    }
  }
}

void DragonBus::readHit(int coreID, int addr) {
  Cache& cache = processors[coreID].cache;
  cache.lruShuffle(addr);
}

void DragonBus::writeHit(int coreID, int addr) {
  if (broadcastingBlocks.find(getHeadAddr(addr)) != broadcastingBlocks.end()) {
    earlyRet = true;
    return;
  }

  int cacheID = coreID;
  Cache& cache = processors[coreID].cache;
  auto state = cache.getBlockState(addr);
  cache.lruShuffle(addr);

  if (state == CacheLine::SHARED || state == CacheLine::SHARED_MODIFIED) {
    // Check if cache should transition to 'M' or 'Sm'
    int countHold = countOthCacheHold(coreID, addr);

    auto addrState = (countHold == 0) ? CacheLine::DIRTY : CacheLine::SHARED_MODIFIED;
    if (addrState == CacheLine::SHARED_MODIFIED) {
      // Broadcast the modified word to other caches
      broadcastWOthCache(cacheID, addr, curTime);
    }
    cache.setBlockState(addr, addrState);
  }

  if (state == CacheLine::EXCLUSIVE) {
    // Transition to 'M'
    cache.setBlockState(addr, CacheLine::DIRTY);
  }

  int blockNum = addr / blockSize;
  invalidBlock[blockNum] = -2;
}

void DragonBus::readMiss(int coreID, int addr) {
  int countHold = countOthCacheHold(coreID, addr);
  auto state = (countHold == 0) ? CacheLine::DIRTY : CacheLine::SHARED;
  cacheReceiveB(coreID, addr, state);
}

void DragonBus::writeMiss(int coreID, int addr) {
  int cacheID = coreID;
  Cache& cache = processors[coreID].cache;
  int countHold = countOthCacheHold(coreID, addr);

  auto state = (countHold == 0) ? CacheLine::DIRTY : CacheLine::SHARED_MODIFIED;

  if (state == CacheLine::DIRTY) {
    cacheReceiveB(coreID, addr, CacheLine::DIRTY);
  } else {
    cacheReceiveB(coreID, addr, CacheLine::SHARED_MODIFIED);

    int sendTime = cache.get(addr).validFrom;
    broadcastWOthCache(cacheID, addr, sendTime);
  }

  int blockNum = addr / blockSize;
  invalidBlock[blockNum] = -2;
}
}// namespace CacheSim
