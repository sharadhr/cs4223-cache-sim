#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <ranges>
#include <string>

#include "BusMonitor.hpp"
#include "Cache.hpp"

namespace CacheSim {
class Bus {
 public:
  BusMonitor monitor{};

  virtual uint32_t getBlockedCycles(CacheOp, uint32_t address, uint8_t drop_pid) = 0;
  virtual void transition(uint8_t pid, uint32_t address) = 0;
  virtual void handleEviction(uint8_t pid, uint32_t address) = 0;

  void printDebug([[maybe_unused]] uint32_t pid, [[maybe_unused]] uint32_t address) {
#ifndef NDEBUG
    std::cout << pid << "\t" << address << "\t";
    std::ranges::for_each(caches, [address](const std::shared_ptr<Cache>& cache) {
      std::cout << std::to_string(static_cast<uint32_t>(cache->getState(address)));
    });
    std::cout << std::endl;
#endif
  }

  void updateDataAccessCount(uint32_t address) {
    bool isShared = false;
    for (auto& cache : caches) {
      if (cache->containsAddress(address)) {
        auto state = cache->getState(address);
        switch (state) {
          case State::OWNED:
          case State::SHARED:
          case State::SHARED_MODIFIED: {
            isShared = true;
            break;
          }
          case State::MODIFIED:
          case State::EXCLUSIVE:
          case State::INVALID:
            break;
        }
      }
    }
    if (isShared) {
      monitor.sharedAccessCount++;
    } else {
      monitor.privateAccessCount++;
    }
  }

  bool inline doOtherCachesContain(uint8_t drop_pid, uint32_t address) {
    return std::ranges::any_of(caches, [&](std::shared_ptr<Cache>& cachePtr) {
      return cachePtr->containsAddress(address) && cachePtr->pid != drop_pid;
    });
  };

  auto inline otherCachesContaining(uint8_t drop_pid, uint32_t address) {
    std::vector<std::shared_ptr<Cache>> returnable{};
    std::ranges::copy_if(caches, std::back_inserter(returnable), [&](auto& cachePtr) {
      return cachePtr->containsAddress(address) && cachePtr->pid != drop_pid;
    });
    return returnable;
  };

  Bus() = default;
  explicit Bus(uint16_t blockSize, std::array<std::shared_ptr<Cache>, 4>& caches) :
      blockSize(blockSize),
      caches(caches){};
  virtual ~Bus() = default;

 protected:
  uint16_t blockSize{32};
  std::array<std::shared_ptr<Cache>, 4> caches;
};

class MESIBus : public Bus {
 public:
  MESIBus() = default;
  ~MESIBus() override = default;
  explicit MESIBus(uint16_t blockSize, std::array<std::shared_ptr<Cache>, 4>&& caches) : Bus(blockSize, caches){};

  uint32_t getBlockedCycles(CacheOp, uint32_t address, uint8_t drop_pid) override;
  void transition(uint8_t pid, uint32_t address) override;
  void handleEviction(uint8_t pid, uint32_t address) override;
};

class DragonBus : public Bus {
 public:
  DragonBus() = default;
  ~DragonBus() override = default;
  explicit DragonBus(uint16_t blockSize, std::array<std::shared_ptr<Cache>, 4>&& caches) : Bus(blockSize, caches){};

  uint32_t getBlockedCycles(CacheOp cacheOp, uint32_t address, uint8_t drop_pid) override;
  void transition(uint8_t pid, uint32_t address) override;
  void handleEviction(uint8_t pid, uint32_t address) override;
};

class MOESIBus : public Bus {
 public:
  MOESIBus() = default;
  ~MOESIBus() override = default;
  explicit MOESIBus(uint16_t blockSize, std::array<std::shared_ptr<Cache>, 4>&& caches) : Bus(blockSize, caches){};

  uint32_t getBlockedCycles(CacheOp, uint32_t address, uint8_t drop_pid) override;
  void transition(uint8_t pid, uint32_t address) override;
  void handleEviction(uint8_t pid, uint32_t address) override;
};
}// namespace CacheSim
