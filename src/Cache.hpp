#pragma once

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
    INVALID,
    MODIFIED,
    SHARED,
    EXCLUSIVE,
    SHARED_MODIFIED,
    OWNED,
  };

 private:
  CacheState state{CacheState::INVALID};
  uint32_t blockNum{};

  CacheLine() : state(CacheState::INVALID) {}
  CacheLine(CacheState state, uint32_t blockNum) : state(state), blockNum(blockNum) {}

  friend class Cache;
};

using State = CacheLine::CacheState;

class Cache {
 public:
  uint8_t pid{};
  CacheOp blockingOperation{CacheOp::PR_RD_MISS};

  Cache() = default;
  Cache(uint8_t pid, uint8_t associativity, uint32_t numBlocks, uint16_t blockSize) :
      pid(pid),
      blockSize(blockSize),
      numBlocks(numBlocks),
      numSets(numBlocks / associativity),
      associativity(associativity) {
    store = std::vector(numBlocks / associativity, std::vector(associativity, CacheLine()));
  }

  void setBlocked(uint32_t address, CacheOp operation);
  bool needsEvictionFor(uint32_t incomingAddress);
  void evictFor(uint32_t incomingAddress);
  CacheOp getCacheOpFor(const Type type, uint32_t address);

  bool containsAddress(uint32_t address);
  void insertLine(uint32_t address, State state);
  void updateLine(uint32_t address, State state);
  void removeLine(uint32_t address);
  void lruShuffle(uint32_t address);
  State getState(uint32_t address);

 private:
  uint16_t blockSize{32};
  uint32_t numBlocks{4096 / 32};
  uint32_t numSets{1};
  uint8_t associativity{2};
  uint32_t blockingCacheBlock{};
  std::vector<std::vector<CacheLine>> store;

  uint8_t getBlockWay(uint32_t blockNum);
  uint8_t getBlockWaySus(uint32_t blockNum);

  bool containsBlock(uint32_t blockNum);

  [[nodiscard]] inline std::vector<CacheLine>& setOfBlock(uint32_t blockNum) { return store[blockNum % numSets]; }

  [[nodiscard]] inline uint32_t setIndexFromAddress(uint32_t address) const { return (address / blockSize) % numSets; }
};
}// namespace CacheSim
