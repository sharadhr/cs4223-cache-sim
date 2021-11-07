#ifndef CS4223_CACHE_SIM_BUS_HPP
#define CS4223_CACHE_SIM_BUS_HPP

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "Cache.hpp"
#include "Processor.hpp"

namespace CacheSim {
struct BusMonitor {
  int trafficData;
  int invalidateCount;
  int updateCount;
  int writebackCount;
  BusMonitor() = default;
};

class Bus {
 public:
  BusMonitor monitor;

  bool earlyRet;

  int blockSize;
  int curTime;

  std::unordered_map<int, int> invalidBlock;
  std::unordered_map<int, int> activeBlocks;

  std::vector<Processor>& processors;

  int getMemBlockAvailableTime(int blockNum);
  void writeBackMem(int cacheID, int addr);
  void checkMem();
  bool checkReleaseCore();
  bool checkCoreReq();
  int getHeadAddr(int addr);

 public:
  Bus(int cacheSize, int assoc, int blockSize,
      std::vector<std::vector<std::pair<int, int>>> coreTraces);

  virtual void readHit(int processorId, int addr) = 0;
  virtual void readMiss(int processorId, int addr) = 0;
  virtual void writeHit(int processorId, int addr) = 0;
  virtual void writeMiss(int processorId, int addr) = 0;

  Bus(int blockSize, std::vector<Processor>& processors) : blockSize(blockSize),
                                                           processors(processors) {
    monitor = BusMonitor();
  }
};

class MESIBus : Bus {
 public:
  void invalidate(int cacheID, int addr, bool needWriteBack);
  void readHit(int processorId, int addr) override;
  void readMiss(int processorId, int addr) override;
  void writeHit(int processorId, int addr) override;
  void writeMiss(int processorId, int addr) override;
  bool cacheAllocAddr(int cacheID, int addr, CacheLine::CacheState addrState);
};

class DragonBus : Bus {
 public:
  std::unordered_map<int, int> broadcastingBlocks;

  int countOthCacheHold(int cacheID, int addr);
  void cacheReceiveW(int cacheID, int addr, int sendCycle);
  void cacheReceiveB(int cacheID, int addr, CacheLine::CacheState state);

  int findMemSourceAvailableTime(int addr);
  int findCacheSourceAvailableTime(int cacheID, int addr);
  int findSourceAvailableTime(int cacheID, int addr);
  void broadcastWOthCache(int cacheID, int addr, int sendCycle);

  void readHit(int coreID, int addr) override;
  void writeHit(int coreID, int addr) override;
  void readMiss(int coreID, int addr) override;
  void writeMiss(int coreID, int addr) override;
};
}// namespace CacheSim

#endif//CS4223_CACHE_SIM_BUS_HPP
