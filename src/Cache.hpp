#ifndef CS4223_CACHE_SIM_CACHE_HPP
#define CS4223_CACHE_SIM_CACHE_HPP

#include <cstdint>
#include <vector>

#include "Bus.hpp"

namespace CacheSim {
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
  uint32_t blockId;
  bool isEmpty;
  CacheLine(uint32_t blockId, CacheState state) : blockId(blockId),
                                                  state(state),
                                                  isEmpty(false) {}
  CacheLine() : isEmpty(true) {}
};

struct CacheLock {
  bool isLocked;
  uint32_t blockedUntil;
  uint32_t blockedOnAdress;
  CacheLine line;
};

class Cache : public CacheLock {
 public:
  Bus& bus;

  uint32_t blockSize{32};
  uint32_t totalSets{64};
  uint32_t associativity{2};
  std::vector<std::vector<CacheLine>> store;
  void has(uint32_t addr);
  void put(uint32_t addr);
  void get(uint32_t);
  void evict(uint32_t);
  void memWriteBack();
  void memRead();

  void lruShuffle(uint32_t setNum, uint32_t blockNum);
  void lruShuffle(uint32_t addr);
  Cache(Bus& bus, uint32_t associativity, uint32_t totalSets, uint32_t blockSize) : bus(bus),
                                                                                    blockSize(blockSize),
                                                                                    totalSets(totalSets),
                                                                                    associativity(associativity) {}
  Cache(Bus& bus) : bus(bus) {}
};
}// namespace CacheSim

#endif
