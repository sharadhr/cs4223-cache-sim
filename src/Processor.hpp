//
// Created by sharadh on 20/10/21.
//

#ifndef CS4223_CACHE_SIM_PROCESSOR_HPP
#define CS4223_CACHE_SIM_PROCESSOR_HPP

#include <cstdint>
#include <fstream>
#include <sstream>
#include <utility>

#include "Bus.hpp"
#include "Cache.hpp"
#include "CoreMonitor.hpp"

namespace CacheSim {

struct Instruction {
  enum class InstructionType { LD = 0, ST = 1, ALU = 2, DONE = 3 };
  InstructionType type;
  uint32_t value;
};

class Processor {
 public:
  uint32_t cycleCount{};
  Cache cache;
  Instruction blockingInstruction{Instruction::InstructionType::ALU, 0};
  CoreMonitor monitor{};

  Processor() = default;
  Processor(const std::ifstream& filePathName, uint8_t pid, uint8_t associativity, uint32_t numBlocks,
            uint16_t blockSize) :
      cache(pid, associativity, numBlocks, blockSize) {
    instructionStream << filePathName.rdbuf();
  }
  void runOneCycle();

 private:
  bool isBlocked{};
  uint32_t blockedFor{};
  std::stringstream instructionStream;

  Instruction getNextInstruction();
  void issueNextInstruction();

  friend class System;
};
}// namespace CacheSim

#endif//CS4223_CACHE_SIM_PROCESSOR_HPP
