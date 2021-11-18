#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <queue>
#include <vector>

#include "Instruction.hpp"

namespace CacheSim {
enum class CacheOp {
  PR_RD_HIT,
  PR_WR_HIT,
  PR_RD_MISS,
  PR_WR_MISS,
  PR_WB,
  PR_NULL,
};

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
  uint32_t blockNum{UINT32_MAX};
  uint32_t address{};
  bool isEmpty{true};

  CacheLine() : state(CacheState::INVALID) {}
  CacheLine(CacheState state, uint32_t address, uint32_t blockNum) :
      state(state),
      blockNum(blockNum),
      address(address),
      isEmpty(false) {}

  friend class Cache;
};

using State = CacheLine::CacheState;

class Cache {
 public:
  uint8_t pid{};
  CacheOp blockingOperation{CacheOp::PR_NULL};

  void throwErrorIfSameBlockNum(uint32_t blockNum);

  Cache() = default;
  Cache(uint8_t pid, uint8_t associativity, uint32_t numBlocks, uint16_t blockSize) :
      pid(pid),
      blockSize(blockSize),
      numSets(numBlocks / associativity),
      associativity(associativity) {
    store = std::vector(numSets, std::vector(associativity, CacheLine()));
  }

  bool needsEvictionFor(uint32_t incomingAddress);
  bool needsWriteBack(uint32_t incomingAddress);
  uint32_t evictedBlockFor(uint32_t incomingAddress);
  void setCacheOpFor(const Type& type, uint32_t address);
  void removeLineForBlock(uint32_t blockNum);

  bool containsAddress(uint32_t address);
  bool containsBlock(uint32_t blockNum);
  void insertLine(uint32_t address, State state);
  void updateLine(uint32_t address, State state);
  void updateLineForBlock(uint32_t blockNum, State state);
  void removeLine(uint32_t address);
  void lruShuffle(uint32_t address);
  State getState(uint32_t address);
  State getStateOfBlock(uint32_t address);

 private:
  uint16_t blockSize{32};
  uint32_t numSets{1};
  uint8_t associativity{2};
  std::vector<std::vector<CacheLine>> store;

  uint8_t getBlockWay(uint32_t blockNum);
  uint8_t getBlockWaySus(uint32_t blockNum);
  bool containsBlockSus(uint32_t blockNum);

  [[nodiscard]] inline std::vector<CacheLine>& setOfBlock(uint32_t blockNum) { return store[blockNum % numSets]; }
  [[nodiscard]] inline uint32_t setIndexFromAddress(uint32_t address) const { return (address / blockSize) % numSets; }
};

inline std::ostream& operator<<(std::ostream& os, CacheLine::CacheState state) {
  switch (state) {
    case CacheLine::CacheState::INVALID:
      return os << "I";
    case CacheLine::CacheState::MODIFIED:
      return os << "M";
    case CacheLine::CacheState::SHARED:
      return os << "S";
    case CacheLine::CacheState::EXCLUSIVE:
      return os << "E";
    case CacheLine::CacheState::SHARED_MODIFIED:
      return os << "Sm";
    case CacheLine::CacheState::OWNED:
      return os << "O";
    default:
      return os;
  }
}

}// namespace CacheSim
