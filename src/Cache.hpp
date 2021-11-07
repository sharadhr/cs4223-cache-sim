#ifndef CS4223_CACHE_SIM_CACHE_HPP
#define CS4223_CACHE_SIM_CACHE_HPP

#include <cstdint>
#include <vector>

namespace CacheSim {
class CacheLine {
 public:
  enum CacheState {
    INVALID,
    DIRTY,
    EXCLUSIVE,
    SHARED,
    SHARED_MODIFIED
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
  int getIndexInSet(uint32_t addr);
  int getInvalidLineFromSet(uint32_t setNum);
  int getEvictionCandidateFromSet(uint32_t setNum);

 public:
  void lruShuffle(uint32_t setNum, uint32_t indexInSet);
  void lruShuffle(uint32_t addr);
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
  CacheLine& evict(uint32_t addr);
  bool isAddrDirty(uint32_t addr);
  bool setBlockState(uint32_t addr, CacheLine::CacheState state);
  CacheLine::CacheState getBlockState(uint32_t addr);
  int getHeadAddr(CacheLine line);
};
}// namespace CacheSim

#endif//CS4223_CACHE_SIM_CACHE_HPP
