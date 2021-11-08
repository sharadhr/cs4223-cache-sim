//
// Created by sharadh on 20/10/21.
//

#ifndef CS4223_CACHE_SIM_PROCESSOR_HPP
#define CS4223_CACHE_SIM_PROCESSOR_HPP

#include <cstdint>
#include <fstream>
#include <sstream>

#include "Cache.hpp"

namespace CacheSim {
class Cache;

struct Instruction {
  enum class InstructionType { LD = 0,
                               ST = 1,
                               ALU = 2,
                               DONE = 3 };
  InstructionType type;
  uint32_t value;
};

class Processor {
 public:
  uint32_t pc{};
  Cache cache;

  Instruction blockedInstruction;

  Processor(const std::ifstream& filePathName, uint8_t pid, uint16_t associativity, uint16_t numBlocks,
            uint32_t blockSize, std::shared_ptr<Bus> bus) : cache(pid, bus, associativity, numBlocks / associativity, blockSize),
                                                            pid{pid} {
    instructionStream << filePathName.rdbuf();
  }
  void runOneCycle();

 private:
  bool isBlocked;
  uint32_t blockedFor;

  uint8_t pid{};

  std::stringstream instructionStream;

  Instruction getNextInstruction();

  void issueNextInstruction();
  void finishCurrentInstruction();
};
}// namespace CacheSim

#endif//CS4223_CACHE_SIM_PROCESSOR_HPP
