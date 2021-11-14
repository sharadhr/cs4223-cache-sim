#pragma once

#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>

#include "Cache.hpp"
#include "CoreMonitor.hpp"
#include "Instruction.hpp"

namespace CacheSim {
class Processor {

 public:
  uint8_t pid{};
  uint32_t cycleCount{};
  Instruction blockingInstruction{Type::ALU, 0};
  CoreMonitor monitor{};

  Processor() : cache(std::make_shared<Cache>()) { instructionStream << std::ifstream("data/test_0.data").rdbuf(); };
  Processor(uint8_t pid, const std::ifstream& filePathName, uint8_t associativity, uint32_t numBlocks,
            uint16_t blockSize) :
      pid(pid),
      cache(std::make_shared<Cache>(pid, associativity, numBlocks, blockSize)) {
    instructionStream << filePathName.rdbuf();
  }

  void refresh();
  void fetchInstruction();
  void block(uint32_t blockedCycles);

  void printData();

  [[nodiscard]] inline CacheOp getCacheOp() const {
    auto result = cache->getCacheOpFor(blockingInstruction.type, blockingInstruction.value);
    return result;
  }

  [[nodiscard]] inline bool isBlocked() const { return blockedFor > 0; }

 private:
  uint32_t blockedFor{0};
  std::stringstream instructionStream{};
  std::shared_ptr<Cache> cache;

  friend class System;
};
}// namespace CacheSim
