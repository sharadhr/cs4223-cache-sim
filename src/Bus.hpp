#ifndef CS4223_CACHE_SIM_BUS_HPP
#define CS4223_CACHE_SIM_BUS_HPP

#include <array>
#include <cstdint>

#include "Cache.hpp"

namespace CacheSim {
class Bus {
 public:
  virtual std::array<uint32_t, 4> blockCycles(std::array<uint32_t, 4>& currentBlockedCycles, uint8_t pid, CacheOp) = 0;
  virtual std::array<CacheLine::CacheState, 4> transition(std::array<CacheLine::CacheState, 4>& previousCycles,
                                                          uint8_t pid, CacheOp operation) = 0;

  Bus() = default;
  explicit Bus(uint16_t blockSize) : blockSize(blockSize){};
  virtual ~Bus() = default;

 protected:
  uint16_t blockSize{32};
};

class MESIBus : public Bus {
 public:
  MESIBus() = default;
  explicit MESIBus(uint16_t blockSize) : Bus(blockSize){};

  std::array<uint32_t, 4> blockCycles(std::array<uint32_t, 4>& currentBlockedCycles, uint8_t pid, CacheOp) override;
  std::array<CacheLine::CacheState, 4> transition(std::array<CacheLine::CacheState, 4>& previousCycles, uint8_t pid,
                                                  CacheOp operation) override;
};

class DragonBus : public Bus {
 public:
  DragonBus() = default;
  explicit DragonBus(uint16_t blockSize) : Bus(blockSize){};

  std::array<uint32_t, 4> blockCycles(std::array<uint32_t, 4>& currentBlockedCycles, uint8_t pid, CacheOp) override;
  std::array<CacheLine::CacheState, 4> transition(std::array<CacheLine::CacheState, 4>& previousCycles, uint8_t pid,
                                                  CacheOp operation) override;
};
}// namespace CacheSim

#endif
