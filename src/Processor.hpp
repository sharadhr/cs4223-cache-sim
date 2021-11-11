//
// Created by sharadh on 20/10/21.
//

#ifndef CS4223_CACHE_SIM_PROCESSOR_HPP
#define CS4223_CACHE_SIM_PROCESSOR_HPP

#include <cstdint>
#include <fstream>
#include <sstream>

#include "Cache.hpp"
#include "CoreMonitor.hpp"

namespace CacheSim {

struct Instruction {
  enum class InstructionType { LD, ST, ALU, DONE };
  InstructionType type;
  uint32_t value;
};

class Processor {
 public:
  uint8_t pid{};
  uint32_t cycleCount{};
  std::shared_ptr<Cache> cache;
  Instruction blockingInstruction{Instruction::InstructionType::ALU, 0};
  CoreMonitor monitor{};

  Processor() : cache() {
    instructionStream << std::ifstream("data/test_0.data").rdbuf();
    blockingInstruction = fetchInstruction();
  };
  Processor(uint8_t pid, const std::ifstream& filePathName, uint8_t associativity, uint32_t numBlocks,
            uint16_t blockSize) :
      pid(pid),
      cache(std::make_shared<Cache>(associativity, numBlocks, blockSize)) {
    instructionStream << filePathName.rdbuf();
    blockingInstruction = fetchInstruction();
  }
  void refresh();

 private:
  uint32_t blockedFor{0};
  std::stringstream instructionStream{};

  Instruction fetchInstruction();

  friend class System;
};
}// namespace CacheSim

#endif//CS4223_CACHE_SIM_PROCESSOR_HPP
