#ifndef CS4223_CACHE_SIM_BUS_HPP
#define CS4223_CACHE_SIM_BUS_HPP

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "Cache.hpp"

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

  int blockSize;
  int curTime;

  std::unordered_map<int, int> invalidBlock;
  std::unordered_map<int, int> activeBlocks;

  int getMemBlockAvailableTime(int blockNum);
  void writeBackMem(int cacheID, int addr);
  void checkMem();
  bool checkReleaseCore();
  bool checkCoreReq();

 public:
  Bus(int cacheSize, int assoc, int blockSize,
      std::vector<std::vector<std::pair<int, int>>> coreTraces);

  virtual void readHit(int processId, int addr) = 0;
  virtual void readMiss(int processId, int addr) = 0;
  virtual void writeHit(int processId, int addr) = 0;
  virtual void writeMiss(int processId, int addr) = 0;

  Bus(int blockSize) : blockSize(blockSize) {
    monitor = BusMonitor();
  }
};

class MESIBus : Bus {
 public:
  void invalidate(int processId, int addr);
  void readHit(int processId, int addr) override;
  void readMiss(int processId, int addr) override;
  void writeHit(int processId, int addr) override;
  void writeMiss(int processId, int addr) override;
};

class DragonBus : Bus {
 public:
  std::unordered_map<int, int> broadcastingBlocks;

  int countOthCacheHold(int cacheID, int addr);
  void cacheReceiveW(int cacheID, int addr, int sendCycle);
  void cacheReceiveB(int cacheID, int addr, CacheSim::CacheLine state);

  int findMemSourceAvailableTime(int cacheID, int addr);
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
