#ifndef CS4223_CACHE_SIM_CACHE_HPP
#define CS4223_CACHE_SIM_CACHE_HPP

#include <cstdint>
#include <memory>
#include <vector>

#include "Bus.hpp"

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
  bool isEmpty{true};
  CacheLine(uint32_t blockNum, CacheState state) : state(state), blockNum(blockNum), isEmpty(false) {}
  CacheLine() : isEmpty(true) {}
};

class Cache {
 public:
  uint8_t pid{};
  std::shared_ptr<Bus> bus;
  uint16_t blockSize{32};
  uint32_t numBlocks{4096 / 32};
  uint8_t associativity{2};

  bool isBlocked{false};
  uint32_t blockedFor{};
  uint32_t blockedOnAddress{};
  CacheOp blockOperation{CacheOp::PR_RD_MISS};
  CacheLine newLine{};

  std::vector<std::vector<CacheLine>> store;

  bool has(uint32_t address);

  void prWr(uint32_t address);
  void prRd(uint32_t);

  CacheLine getLine(uint32_t);

  void evict(uint32_t);
  void memWriteBack();
  void memRead();
  void issueBusTransaction();

  void update();

  void block(uint32_t address, uint32_t blockedForCycles, CacheOp blockOp);

  CacheLine createNewLine(uint32_t address, CacheLine::CacheState state) const;

  uint32_t getIndexOfBlockInSet(uint32_t blockNum);

  void lruShuffle(uint32_t setIndex, uint32_t blockNum);
  void lruShuffle(uint32_t address);

  Cache() = default;
  Cache(uint8_t pid, std::shared_ptr<Bus>& bus, uint8_t associativity, uint32_t numBlocks, uint16_t blockSize) :
      pid(pid),
      bus(bus),
      associativity(associativity),
      blockSize(blockSize),
      numBlocks(numBlocks) {}
};
}// namespace CacheSim

#endif
