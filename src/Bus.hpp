#ifndef CS4223_CACHE_SIM_BUS_HPP
#define CS4223_CACHE_SIM_BUS_HPP

#include <array>
#include <cstdint>

#include "Cache.hpp"

namespace CacheSim {
class Bus {
 public:
  virtual uint32_t blockedCycles(std::array<std::shared_ptr<Cache>, 4>& caches, uint8_t pid, CacheOp) = 0;
  virtual void transition(std::array<std::shared_ptr<Cache>, 4>& caches, uint8_t pid) = 0;

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

  uint32_t blockedCycles(std::array<std::shared_ptr<Cache>, 4>& caches, uint8_t pid, CacheOp) override;
  void transition(std::array<std::shared_ptr<Cache>, 4>& caches, uint8_t pid) override;
};

class DragonBus : public Bus {
 public:
  DragonBus() = default;
  explicit DragonBus(uint16_t blockSize) : Bus(blockSize){};

  uint32_t blockedCycles(std::array<std::shared_ptr<Cache>, 4>& caches, uint8_t pid, CacheOp) override;
  void transition(std::array<std::shared_ptr<Cache>, 4>& caches, uint8_t pid) override;
};
}// namespace CacheSim

#endif
