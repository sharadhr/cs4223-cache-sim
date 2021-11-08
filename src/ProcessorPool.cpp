//
// Created by sharadh on 20/10/21.
//

#include "ProcessorPool.hpp"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>

#include "Bus.hpp"

namespace CacheSim {
ProcessorPool::ProcessorPool(const std::filesystem::path& benchmark, std::string_view protocol, uint16_t associativity, uint16_t numBlocks, uint16_t blockSize) {
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

    if (protocol == "MESI") {
      auto bus = MESIBus(processors);
      processor = Processor(std::ifstream(coreBenchmarkFile), pid++, associativity, numBlocks, blockSize, std::make_shared<Bus>(bus));
    } else {
      auto bus = DragonBus(processors);
      processor = Processor(std::ifstream(coreBenchmarkFile), pid++, associativity, numBlocks, blockSize, std::make_shared<Bus>(bus));
    }
  }
}

bool ProcessorPool::processorsDone() {
  for (auto& processor : processors) {
    if (processor.blockedInstruction.type != Instruction::InstructionType::DONE) {
      return false;
    }
  }
  return true;
}

void ProcessorPool::run() {
  while (!processorsDone()) {
    for (auto& processor : processors) {
      processor.runOneCycle();
    }
  }

  // return {processors[0].}
}
}// namespace CacheSim
