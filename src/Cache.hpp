#ifndef CS4223_CACHE_SIM_CACHE_HPP
#define CS4223_CACHE_SIM_CACHE_HPP

#include <cstdint>
#include <memory>
#include <queue>
#include <vector>

namespace CacheSim {
enum class CacheOp { PR_RD_HIT, PR_WR_HIT, PR_RD_MISS, PR_WR_MISS, PR_WB };

class CacheLine {
 public:
  enum class CacheState {
    MODIFIED,
    SHARED,
    SHARED_MODIFIED,
    INVALID,
    EXCLUSIVE,
  };

 private:
  CacheState state{CacheState::INVALID};
  uint32_t blockNum{};
  bool dirty{};

  CacheLine() : state(CacheState::INVALID) {}
  CacheLine(CacheState state, uint32_t blockNum) : state(state), blockNum(blockNum) {}

  friend class Cache;
};

class Cache {
 public:
  Cache() = default;
  Cache(uint8_t associativity, uint32_t numBlocks, uint16_t blockSize) :
      blockSize(blockSize),
      numBlocks(numBlocks),
      associativity(associativity) {
    store = std::vector(numBlocks / associativity, std::vector(associativity, CacheLine()));
  }

  void block(uint32_t address, uint32_t blockedCycles, CacheOp operation);
  void refresh();
  bool needsEviction(uint32_t incomingAddress);
  void evictFor(uint32_t incomingAddress);
  bool has(uint32_t address);

 private:
  uint16_t blockSize{32};
  uint32_t numBlocks{4096 / 32};
  uint8_t associativity{2};
  uint32_t blockedFor{};
  uint32_t blockedOnAddress{};
  bool needsWriteback{};
  CacheOp blockingOperation{CacheOp::PR_RD_MISS};
  std::vector<std::vector<CacheLine>> store;

  CacheLine getLine(uint32_t address);
  void lruShuffle(uint32_t address);
  uint32_t getIndexOfBlockInSet(uint32_t);
  CacheOp readWriteOperation(uint32_t address);

  friend class System;
  friend class Processor;
};
}// namespace CacheSim

#endif
