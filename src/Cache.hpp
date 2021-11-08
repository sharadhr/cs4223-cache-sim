#ifndef CS4223_CACHE_SIM_CACHE_HPP
#define CS4223_CACHE_SIM_CACHE_HPP

#include <cstdint>
#include <memory>
#include <vector>

namespace CacheSim {
class Bus;

enum CacheOp {
  PR_RD_HIT,
  PR_WR_HIT,
  PR_RD_MISS,
  PR_WR_MISS
};

class CacheLine {
 public:
  enum CacheState {
    MODIFIED,
    SHARED,
    SHARED_MODIFIED,
    INVALID,
    EXCLUSIVE,
  };
  CacheState state;
  uint32_t blockNum;
  bool isEmpty;
  CacheLine(uint32_t blockNum, CacheState state) : state(state),
                                                   blockNum(blockNum),
                                                   isEmpty(false) {}
  CacheLine() : isEmpty(true) {}
};

class Cache {
 public:
  uint32_t processorId;

  std::shared_ptr<Bus> bus;

  bool isBlocked{false};
  uint32_t blockedFor{0};
  uint32_t blockedOnAdress;
  CacheOp blockOperation;
  CacheLine newLine;

  uint32_t blockSize{32};
  uint32_t totalSets{64};
  uint32_t associativity{2};
  std::vector<std::vector<CacheLine>> store;

  bool has(uint32_t addr);

  void prWr(uint32_t addr);
  void prRd(uint32_t);

  CacheLine& getLine(uint32_t);

  void evict(uint32_t);
  void memWriteBack();
  void memRead();
  void issueBusTransaction();

  void update();

  void block(uint32_t addr, uint32_t blockedFor, CacheOp blockOp);

  CacheLine createNewLine(uint32_t addr, CacheLine::CacheState state);

  uint32_t getIndexOfBlockInSet(uint32_t blockNum);

  void lruShuffle(uint32_t setNum, uint32_t blockNum);
  void lruShuffle(uint32_t addr);
  Cache(uint32_t processorId, std::shared_ptr<Bus> bus, uint32_t associativity, uint32_t totalSets, uint32_t blockSize) : processorId(processorId),
                                                                                                                          bus(bus),
                                                                                                                          blockSize(blockSize),
                                                                                                                          totalSets(totalSets),
                                                                                                                          associativity(associativity) {}
};
}// namespace CacheSim

#endif
