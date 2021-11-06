#ifndef CS4223_CACHE_SIM_PROCESSOR_HPP
#define CS4223_CACHE_SIM_PROCESSOR_HPP

#include <cstdint>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>

namespace CacheSim {
class CacheLine {
 public:
  enum CacheState {
    INVALID,
    DIRTY
  };

  // INVALID, DIRTY
  CacheState state;
  int blockNumber;
  int validFrom;
  CacheLine(CacheState state = INVALID, int blockNumber = -1, int validFrom = -1) : state(state),
                                                                                    blockNumber(blockNumber),
                                                                                    validFrom(validFrom){};
};

class Cache {
 private:
  void lruShuffle(uint32_t setNum, uint32_t indexInSet);
  int getIndexInSet(uint32_t addr);
  int getInvalidLineFromSet(uint32_t setNum);
  int getEvictionCandidateFromSet(uint32_t setNum);

 public:
  uint16_t associativity;
  uint16_t totalSets;
  uint16_t blockSize;
  std::vector<std::vector<CacheLine>> store;

  // 4KB 32-byte 2-way set associative -> totalSets = 4 * 1024 / 2 / 32 = 64 blocks
  Cache(uint16_t associativity = 2, uint16_t totalSets = 64,
        uint32_t blockSize = 32) : associativity(associativity),
                                   totalSets(totalSets),
                                   blockSize(blockSize) {
    this->store = std::vector<std::vector<CacheLine>>(totalSets,
                                                      std::vector<CacheLine>(associativity, CacheLine()));
  }

  bool has(uint32_t addr);
  CacheLine& get(uint32_t addr);
  void put(uint32_t addr, CacheLine::CacheState state, int validFrom);
  void evict(uint32_t addr);
};
}// namespace CacheSim

#endif//CS4223_CACHE_SIM_PROCESSOR_HPP
