//
// Created by sharadh on 20/10/21.
//

#ifndef CS4223_CACHE_SIM_PROCESSOR_HPP
#define CS4223_CACHE_SIM_PROCESSOR_HPP

#include <stdint.h>
#include <cstdint>
#include <fstream>
#include <string_view>

namespace CacheSim {
struct Instruction {
  enum class InstructionType { LD = 0,
                         ST = 1,
                         ALU = 2 };
  InstructionType type;
  uint32_t value;
};

class Processor {
 public:
  uint32_t pc{};
  Processor() = default;
  Processor(const std::string& filePath, uint8_t pid, uint16_t associativity, uint16_t numBlocks,
            uint32_t blockSize) : pid{pid},
                                  instructionStream(filePath) {}
  // cache(associativity, numBlocks, blockSize),
  // monitor() {}
  uint32_t runNextInstruction();

 private:
  uint8_t pid{};

  std::ifstream instructionStream;
  // CacheSim::CacheController cache;
  // CacheSim::CoreMonitor monitor;

  uint16_t execute(const Instruction& otherInst);
  uint16_t readFrom(const Instruction& readInst);
  uint16_t writeBack(const Instruction& writeInst);
};
}// namespace CacheSim

#endif//CS4223_CACHE_SIM_PROCESSOR_HPP
