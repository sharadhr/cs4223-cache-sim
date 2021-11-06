//
// Created by sharadh on 20/10/21.
//

#include "ProcessorPool.hpp"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace CacheSim {
void ProcessorPool::setup(const std::filesystem::path& benchmark, uint16_t associativity, uint16_t numBlocks, uint16_t blockSize) {
  if (benchmark.empty())
    throw std::domain_error("Benchmark file name is empty.");

  uint8_t pid = 0;
  for (auto& processor : processors) {
    auto coreBenchmarkFile = benchmark;
    coreBenchmarkFile += "_" + std::to_string(pid);
    coreBenchmarkFile.replace_extension("data");

    if (!std::filesystem::exists(coreBenchmarkFile))
      throw std::domain_error(coreBenchmarkFile.make_preferred().string() + " does not exist in working directory: "
                              + std::filesystem::current_path().make_preferred().string());

    processor = Processor(std::ifstream(coreBenchmarkFile), pid++, associativity, numBlocks / associativity, blockSize);
  }
}

void ProcessorPool::run() {
  static std::array<uint16_t, 4> blockedCycles;
  int breaker = 0;

  while (true) {
    for (auto& processor : processors) {
      auto exit = processor.runOneCycle();
      if (exit) ++breaker;
    }
    if (breaker == processors.size()) break;
  }

  // return {processors[0].}
}
}// namespace CacheSim
