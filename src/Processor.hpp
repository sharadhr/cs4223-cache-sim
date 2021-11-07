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
class Core {
 public:
  enum CoreState {
    FREE = 0,
    BUSY = 1,
    BUSY_WAIT = 2
  };

  Core() = default;

  CoreState state;
  int nextFree;

  void setBusy(int nextFree);

  void refresh(int curTime);
};

struct Instruction {
  enum class InstructionType { LD = 0,
                               ST = 1,
                               ALU = 2 };
  InstructionType type;
  uint32_t value;
};

struct ProcessorMonitor {
  int lastBusAccess;
  int execCycles;
  int compCycles;
  int idleCycles;
  int loadCount;
  int storeCount;
  int cacheMissCount;
  int privateAccessCount;

  ProcessorMonitor() = default;
};

class Processor : public Core {

 public:
  uint32_t pc{};
  bool isDone();

  Cache cache;

  ProcessorMonitor processorMonitor;

  std::vector<Instruction> instructions;

  Processor() = default;
  Processor(const std::ifstream& filePathName, uint8_t pid, uint16_t associativity, uint16_t numBlocks,
            uint32_t blockSize) : pid{pid} {
    processorMonitor = ProcessorMonitor();
    instructionStream << filePathName.rdbuf();
    cache = Cache(associativity, numBlocks / associativity, blockSize);
    loadInstructions();
  }
  void run(int cycles = 1);

 private:
  uint8_t pid{};

  std::stringstream instructionStream;

  void loadInstructions();
};
}// namespace CacheSim

#endif//CS4223_CACHE_SIM_PROCESSOR_HPP
