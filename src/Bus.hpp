#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <ranges>

#include "BusMonitor.hpp"
#include "Cache.hpp"

namespace CacheSim {
class Bus {
 public:
  BusMonitor monitor{};

  virtual uint32_t getBlockedCycles(std::array<std::shared_ptr<Cache>, 4>&& caches, CacheOp, uint32_t address,
                                    uint8_t drop_pid) = 0;
  virtual void transition(std::array<std::shared_ptr<Cache>, 4>&& caches, uint8_t pid, uint32_t address) = 0;
  virtual void handleEviction(std::array<std::shared_ptr<Cache>, 4>&& caches, uint8_t pid, uint32_t blockNum) = 0;

  void printDebug(std::array<std::shared_ptr<Cache>, 4>& caches, uint32_t pid, uint32_t address) {
#ifndef NDEBUG
    std::cout << pid << "\t" << address << "\t";
    std::for_each(caches.begin(), caches.end(),
                  [address](const std::shared_ptr<Cache> cache) { std::cout << (int) cache->getState(address); });
    std::cout << std::endl;
#endif
  }

  bool inline doOtherCachesContain(std::array<std::shared_ptr<Cache>, 4>& caches, uint8_t drop_pid, uint32_t address) {
    return std::ranges::any_of(caches, [&](std::shared_ptr<Cache>& cachePtr) {
      return cachePtr->containsAddress(address) && cachePtr->pid != drop_pid;
    });
  };

  auto inline otherCachesContaining(std::array<std::shared_ptr<Cache>, 4>& caches, uint8_t drop_pid, uint32_t address) {
    std::vector<std::shared_ptr<Cache>> returnable{};
    std::ranges::copy_if(caches, std::back_inserter(returnable), [&](std::shared_ptr<Cache>& cachePtr) {
      return cachePtr->containsAddress(address) && cachePtr->pid != drop_pid;
    });
    return returnable;
  };

  Bus() = default;
  explicit Bus(uint16_t blockSize) : blockSize(blockSize){};
  virtual ~Bus() = default;

 protected:
  uint16_t blockSize{32};
  std::array<std::shared_ptr<Cache>, 4> busCaches;
};

class MESIBus : public Bus {
 public:
  MESIBus() = default;
  ~MESIBus() override = default;
  explicit MESIBus(uint16_t blockSize) : Bus(blockSize){};

  uint32_t getBlockedCycles(std::array<std::shared_ptr<Cache>, 4>&& caches, CacheOp, uint32_t,
                            uint8_t drop_pid) override;
  void transition(std::array<std::shared_ptr<Cache>, 4>&& caches, uint8_t pid, uint32_t address) override;
  void handleEviction(std::array<std::shared_ptr<Cache>, 4>&& caches, uint8_t pid, uint32_t blockNum) override;
};

class DragonBus : public Bus {
 public:
  DragonBus() = default;
  ~DragonBus() override = default;
  explicit DragonBus(uint16_t blockSize) : Bus(blockSize){};

  uint32_t getBlockedCycles(std::array<std::shared_ptr<Cache>, 4>&& caches, CacheOp cacheOp, uint32_t address,
                            uint8_t drop_pid) override;
  void transition(std::array<std::shared_ptr<Cache>, 4>&& cachePtr, uint8_t pid, uint32_t address) override;
  void handleEviction(std::array<std::shared_ptr<Cache>, 4>&& caches, uint8_t pid, uint32_t blockNum) override;
};

class MOESIBus : public Bus {
 public:
  MOESIBus() = default;
  ~MOESIBus() override = default;
  explicit MOESIBus(uint16_t blockSize) : Bus(blockSize){};

  uint32_t getBlockedCycles(std::array<std::shared_ptr<Cache>, 4>&& caches, CacheOp, uint32_t address,
                            uint8_t drop_pid) override;
  void transition(std::array<std::shared_ptr<Cache>, 4>&& caches, uint8_t pid, uint32_t address) override;
  void handleEviction(std::array<std::shared_ptr<Cache>, 4>&& caches, uint8_t pid, uint32_t blockNum) override;
};
}// namespace CacheSim
