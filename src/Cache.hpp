#ifndef CS4223_CACHE_SIM_CACHE_HPP
#define CS4223_CACHE_SIM_CACHE_HPP

#include <cstdint>
#include <vector>

#include "Bus.hpp"

namespace CacheSim {
class CacheLock {
 public:
  bool isLocked;
  uint32_t blockedUntil;
  uint32_t blockedOnAdress;
};

class CacheLine {
 public:
  enum CacheState {
    INVALID,
    DIRTY,
    EXCLUSIVE
  };
  CacheState state;
  uint32_t blockId;
  bool isEmpty;
  CacheLine(uint32_t blockId, CacheState state) : blockId(blockId),
                                                  state(state),
                                                  isEmpty(false) {}
  CacheLine() : isEmpty(true) {}
};
class Cache : public CacheLock {
 public:
  Bus& bus;

  uint32_t blockSize{32};
  uint32_t totalSets{64};
  uint32_t associativity{2};
  std::vector<std::vector<CacheLine>> store;
  bool has(uint32_t addr);
  bool put(uint32_t addr);
  bool get(uint32_t);
  bool evict(uint32_t);
  bool memWriteBack();
  bool memRead();
  Cache(Bus& bus, uint32_t associativity, uint32_t totalSets, uint32_t blockSize) : bus(bus),
                                                                                    blockSize(blockSize),
                                                                                    totalSets(totalSets),
                                                                                    associativity(associativity) {}
  Cache(Bus& bus) : bus(bus) {}
};
}// namespace CacheSim

#endif
