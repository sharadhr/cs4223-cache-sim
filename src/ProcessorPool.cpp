//
// Created by sharadh on 20/10/21.
//

#include "ProcessorPool.hpp"

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <stdexcept>

namespace CacheSim {
void ProcessorPool::setup(const std::filesystem::path& benchmark, std::string_view protocol, uint16_t associativity,
                          uint16_t numBlocks, uint16_t blockSize) {
  if (benchmark.empty()) throw std::domain_error("Benchmark file name is empty.");

  uint8_t pid = 0;
  for (auto& processor : processors) {
    auto coreBenchmarkFile = benchmark;
    coreBenchmarkFile += "_" + std::to_string(pid);
    coreBenchmarkFile.replace_extension("data");

    if (!std::filesystem::exists(coreBenchmarkFile))
      throw std::domain_error(coreBenchmarkFile.make_preferred().string() + " does not exist in working directory: "
                              + std::filesystem::current_path().make_preferred().string());

    if (protocol == "MESI") {
      std::shared_ptr<Bus> busPtr = std::make_shared<MESIBus>();
      processor = Processor(std::ifstream(coreBenchmarkFile), pid++, associativity, numBlocks, blockSize, busPtr);
    } else {
      std::shared_ptr<Bus> busPtr = std::make_shared<DragonBus>();
      processor = Processor(std::ifstream(coreBenchmarkFile), pid++, associativity, numBlocks, blockSize, busPtr);
    }
  }
}

bool ProcessorPool::processorsDone() {
  return std::ranges::all_of(processors, [](const Processor& processor) {
    return processor.blockingInstruction.type == Instruction::InstructionType::DONE;
  });
}

void ProcessorPool::run() {
  while (!processorsDone())
    for (auto& processor : processors) processor.runOneCycle();
}
}// namespace CacheSim
