#include <cstdint>
#include <iostream>

#include "Cache.hpp"

namespace CacheSim {
struct BusMonitor {
  uint32_t trafficBytes{0};
  uint32_t numOfInvalidationsOrUpdates{0};
  uint32_t privateDataAccessCount{0};
  uint32_t sharedDataAccessCount{0};
};

class Bus {
 public:
  virtual uint32_t getBlockedCycles(std::array<std::shared_ptr<Cache>, 4>&& caches, CacheOp, uint32_t address) = 0;
  virtual void transition(std::array<std::shared_ptr<Cache>, 4>&& caches, uint8_t pid, uint32_t address) = 0;
  void printDebug(std::array<std::shared_ptr<Cache>, 4>&& caches, uint32_t pid, uint32_t address) {
    std::cout << address << " " << pid << " ";
    std::for_each(caches.begin(), caches.end(),
                  [address](const std::shared_ptr<Cache> cache) { std::cout << (int) cache->getState(address); });
    std::cout << std::endl;
  }

  BusMonitor monitor;

  Bus() = default;
  explicit Bus(uint16_t blockSize) : blockSize(blockSize){};
  virtual ~Bus() = default;

 protected:
  uint16_t blockSize{32};
};

class MESIBus : public Bus {
 public:
  MESIBus() = default;
  ~MESIBus() override = default;
  explicit MESIBus(uint16_t blockSize) : Bus(blockSize){};

  uint32_t getBlockedCycles(std::array<std::shared_ptr<Cache>, 4>&& caches, CacheOp, uint32_t) override;
  void transition(std::array<std::shared_ptr<Cache>, 4>&& caches, uint8_t pid, uint32_t address) override;

 private:
};

class DragonBus : public Bus {
 public:
  DragonBus() = default;
  ~DragonBus() override = default;
  explicit DragonBus(uint16_t blockSize) : Bus(blockSize){};

  uint32_t getBlockedCycles(std::array<std::shared_ptr<Cache>, 4>&& caches, CacheOp, uint32_t) override;
  void transition(std::array<std::shared_ptr<Cache>, 4>&& caches, uint8_t pid, uint32_t address) override;

 private:
};

class MOESIBus : public Bus {
 public:
  MOESIBus() = default;
  ~MOESIBus() override = default;
  explicit MOESIBus(uint16_t blockSize) : Bus(blockSize){};

  uint32_t getBlockedCycles(std::array<std::shared_ptr<Cache>, 4>&& caches, CacheOp, uint32_t) override;
  void transition(std::array<std::shared_ptr<Cache>, 4>&& caches, uint8_t pid, uint32_t address) override;

 private:
};
}// namespace CacheSim
