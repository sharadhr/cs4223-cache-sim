//
// Created by sharadh on 20/10/21.
//

#include "ProcessorPool.hpp"

#include <iostream>

namespace CacheSim {
void ProcessorPool::setup(const std::filesystem::path& benchmark, uint16_t associativity, uint16_t numBlocks, uint16_t blockSize) {
  int pid = 0;
  for (auto& processor : processors) {
    auto coreBenchmarkFile = benchmark;
    coreBenchmarkFile += "_" + std::to_string(pid);
    coreBenchmarkFile.replace_extension("data");
    processor = Processor(coreBenchmarkFile.native(), pid++, associativity, numBlocks, blockSize);
  }
}
void ProcessorPool::run() {
  static std::array<uint32_t, 4> blockedCycles;
  int breaker = 0;

  while (true) {
    for (size_t i = 0; i < processors.size(); ++i) {
      if (blockedCycles[i] == 0){
        auto cycles = processors[i].runNextInstruction();
        if (cycles == 0) {
          blockedCycles[i] = UINT32_MAX;
          --breaker;
          std::cout << "Processor " << i << " done, with " << processors[i].pc << " cycles." << std::endl;
          continue;
        }
        blockedCycles[i] = cycles;
      }
      if (blockedCycles[i] != UINT32_MAX) --blockedCycles[i];
    }
    if (breaker == -4) break;
  }
}
}// namespace CacheSim
