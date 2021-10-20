//
// Created by sharadh on 20/10/21.
//

#ifndef CS4223_CACHE_SIM_PROCESSOR_HPP
#define CS4223_CACHE_SIM_PROCESSOR_HPP

#include <cstdint>
#include <fstream>

namespace CacheSim {
struct Instruction {
  enum InstructionType { LD = 0,
                         ST = 1,
                         ALU = 2 };
  InstructionType type;
  uint32_t value;
};

class Processor {
 public:
  uint32_t pc{};
  Processor() = default;
  Processor(std::string_view filePath, int pid, uint32_t associativity, uint32_t numBlocks, uint32_t blockSize) : pid{static_cast<uint8_t>(pid)},
                                                                                                                  instructionStream(filePath.data()) {}
  // cache(associativity, numBlocks, blockSize),
  // monitor() {}
  uint32_t runNextInstruction();

 private:
  uint8_t pid{};

  std::ifstream instructionStream;
  // CacheSim::CacheController cache;
  // CacheSim::CoreMonitor monitor;

  Instruction getNextInstruction();
  uint32_t execute(const Instruction& otherInst);
  uint32_t readFrom(const Instruction& readInst);
  uint32_t writeBack(const Instruction& writeInst);
};
}// namespace CacheSim

#endif//CS4223_CACHE_SIM_PROCESSOR_HPP
