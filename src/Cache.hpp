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
  bool isBlocked{};
  uint32_t blockedFor{};
  uint32_t blockedOnAddress{};
  bool needsEviction{};
  CacheOp blockOperation{CacheOp::PR_RD_MISS};
  std::vector<std::vector<CacheLine>> store;

  void evict(uint32_t);
  void lruShuffle(uint32_t, uint32_t);
  void lruShuffle(uint32_t);

 public:
  CacheLine getLine(uint32_t);
  void block(uint32_t, uint32_t, CacheOp);
  void block(uint32_t, uint32_t);

  uint32_t getIndexOfBlockInSet(uint32_t);

  void update();
  bool has(uint32_t);
  void prWr(uint32_t);
  void prRd(uint32_t);

  Cache() = default;
  Cache(uint8_t associativity, uint32_t numBlocks, uint16_t blockSize) :
      blockSize(blockSize),
      numBlocks(numBlocks),
      associativity(associativity) {
    store = std::vector(numBlocks / associativity, std::vector(associativity, CacheLine()));
  }

  friend class System;
};
}// namespace CacheSim

#endif
