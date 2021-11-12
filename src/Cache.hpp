#ifndef CS4223_CACHE_SIM_CACHE_HPP
#define CS4223_CACHE_SIM_CACHE_HPP

#include <cstdint>
#include <memory>
#include <queue>
#include <vector>

#include "Instruction.hpp"

namespace CacheSim {
enum class CacheOp { PR_RD_HIT, PR_WR_HIT, PR_RD_MISS, PR_WR_MISS, PR_WB, PR_NULL };

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

  CacheLine() : state(CacheState::INVALID) {}
  CacheLine(CacheState state, uint32_t blockNum) : state(state), blockNum(blockNum) {}

  friend class Cache;
};

class Cache {
  using State = CacheLine::CacheState;
 public:
  Cache() = default;
  Cache(uint8_t associativity, uint32_t numBlocks, uint16_t blockSize) :
      blockSize(blockSize),
      numBlocks(numBlocks),
      numSets(numBlocks / associativity),
      associativity(associativity) {
    store = std::vector(numBlocks / associativity, std::vector(associativity, CacheLine()));
  }

  void refresh();
  void setBlocked(uint32_t address, uint32_t blockedCycles, CacheOp operation);
  bool needsEvictionFor(uint32_t incomingAddress);
  void evictFor(uint32_t incomingAddress);
  CacheOp getCacheOpFor(const Type &type, uint32_t address);

 private:
  uint16_t blockSize{32};
  uint32_t numBlocks{4096 / 32};
  uint32_t numSets{1};
  uint8_t associativity{2};
  uint32_t blockedFor{};
  uint32_t blockedOnBlock{};
  CacheOp blockingOperation{CacheOp::PR_RD_MISS};
  std::vector<std::vector<CacheLine>> store;

  void lruShuffle(uint32_t blockNum);
  uint8_t getBlockWay(uint32_t blockNum);
  bool contains(uint32_t blockNum);

  inline uint32_t setIndexFromBlock(uint32_t blockNum) {
    return blockNum % numSets;
  }

  inline uint32_t setIndexFromAddress(uint32_t address) {
    return (address / blockSize) % numSets;
  }

  friend class System;
  friend class Processor;
};
}// namespace CacheSim

#endif
