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
  enum CoreState {
    FREE = 0,
    BUSY = 1
  };

  CoreState state;
  int nextFree;

  void setState(int state);
  void setNextFree(int nextFree);

 public:
  Core() = default;

  void setBusyWait();
  void setBusy(int nextFree);
  void setFree();

  bool isFree();
  bool isBusyWait();
  bool isBusy();

  int getNextFree();
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

  Cache cache;

  ProcessorMonitor processorMonitor;

  Processor() = default;
  Processor(const std::ifstream& filePathName, uint8_t pid, uint16_t associativity, uint16_t numBlocks,
            uint32_t blockSize) : pid{pid} {
    processorMonitor = ProcessorMonitor();
    instructionStream << filePathName.rdbuf();
    cache = Cache(associativity, numBlocks / associativity, blockSize);
  }
  // cache(associativity, numBlocks, blockSize),
  // monitor() {}
  uint32_t runOneCycle();

 private:
  uint8_t pid{};

  std::stringstream instructionStream;
  // CacheSim::CoreMonitor monitor;
};// namespace CacheSim
}// namespace CacheSim

#endif//CS4223_CACHE_SIM_PROCESSOR_HPP
