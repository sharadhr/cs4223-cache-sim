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
void ProcessorPool::setup(Protocol protocol, const std::filesystem::path& benchmark, uint16_t associativity, uint16_t numBlocks, uint16_t blockSize) {
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

    processor = Processor(std::ifstream(coreBenchmarkFile), pid++, associativity, numBlocks, blockSize);
  }

  switch (protocol) {
    case MESI:
      bus = new MESIBus(blockSize, &processors);
      break;
    case DRAGON:
      bus = new DragonBus(blockSize, &processors);
      break;
  }
}

bool ProcessorPool::isDone() {
  for (int i = 0; i < (int) processors.size(); i++) {
    if (!processors.at(i).isDone()) {
      return false;
    }
  }
  return true;
}

void ProcessorPool::run(int cycles) {
  while (!isDone()) {
    for (auto& processor : processors) {
      processor.run(cycles);
    }
    bus->run(cycles);
  }
}
}// namespace CacheSim
