//
// Created by sharadh on 20/10/21.
//

#ifndef CS4223_CACHE_SIM_PROCESSOR_HPP
#define CS4223_CACHE_SIM_PROCESSOR_HPP

#include <cstdint>
#include <fstream>
#include <sstream>

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
  Processor(const std::ifstream& filePathName, uint8_t pid, uint16_t associativity, uint16_t numBlocks,
            uint32_t blockSize) : pid{pid} {
    instructionStream << filePathName.rdbuf();
  }
  // cache(associativity, numBlocks, blockSize),
  // monitor() {}
  uint32_t runOneCycle();

 private:
  uint8_t pid{};

  std::stringstream instructionStream;
  // CacheSim::CacheController cache;
  // CacheSim::CoreMonitor monitor;
};
}// namespace CacheSim

#endif//CS4223_CACHE_SIM_PROCESSOR_HPP
