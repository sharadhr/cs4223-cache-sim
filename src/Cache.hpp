#ifndef CS4223_CACHE_SIM_CACHE_HPP
#define CS4223_CACHE_SIM_CACHE_HPP

#include <cstdint>
#include <memory>
#include <queue>
#include <vector>

namespace CacheSim {
enum class CacheOp { PR_RD_HIT, PR_WR_HIT, PR_RD_MISS, PR_WR_MISS };

class CacheLine {
 public:
  enum class CacheState {
    MODIFIED,
    SHARED,
    SHARED_MODIFIED,
    INVALID,
    EXCLUSIVE,
  };

  CacheState state{CacheState::INVALID};
  uint32_t blockNum{};
  CacheLine(uint32_t blockNum, CacheState state) : state(state), blockNum(blockNum) {}
  CacheLine() : state(CacheState::INVALID) {}
};

class Cache {
 private:
  uint16_t blockSize{32};
  uint32_t numBlocks{4096 / 32};
  uint8_t associativity{2};

  // lruShuffle, writeback write-allocate
  // blockedFor for eviction
  void evict(uint32_t);

  void lruShuffle(uint32_t, uint32_t);
  void lruShuffle(uint32_t);

 public:
  uint8_t pid{};

  bool isBlocked{false};
  uint32_t blockedFor{};
  uint32_t blockedOnAddress{};
  CacheOp blockOperation{CacheOp::PR_RD_MISS};

  std::vector<std::vector<CacheLine>> store;

  CacheLine getLine(uint32_t);

  void block(uint32_t, uint32_t, CacheOp);

  void block(uint32_t, uint32_t);

  CacheLine createNewLine(uint32_t, CacheLine::CacheState) const;

  uint32_t getIndexOfBlockInSet(uint32_t);

  bool needsEviction{false};

  void update();

  bool has(uint32_t);

  // lruShuffle
  void prWr(uint32_t);

  // lruShuffle
  void prRd(uint32_t);

  Cache() = default;
  Cache(uint8_t pid, uint8_t associativity, uint32_t numBlocks, uint16_t blockSize) :
      pid(pid),
      blockSize(blockSize),
      numBlocks(numBlocks),
      associativity(associativity) {
    store = std::vector(numBlocks / associativity, std::vector(associativity, CacheLine()));
  }
};
}// namespace CacheSim

//
// get(CacheLine1)
// CacheLine2, CacheLine1
//
// p2.cache.prWr(address)
// p1.flush(address)
//
// CacheLine1: invalidated, CacheLine2: Valid,
//
// p1.cache.get(address) RdMiss -> p2.cache.has(address)
//

#endif
