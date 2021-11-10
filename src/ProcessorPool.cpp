//
// Created by sharadh on 20/10/21.
//

#include "ProcessorPool.hpp"

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <stdexcept>

#include "Processor.hpp"

namespace CacheSim {
void System::setup(const std::filesystem::path& benchmark, std::string_view protocol, uint8_t associativity,
                   uint32_t numBlocks, uint16_t blockSize) {
  if (benchmark.empty()) throw std::domain_error("Benchmark file name is empty.");

  uint8_t pid = 0;
  for (auto& processor : processors) {
    auto coreBenchmarkFile = benchmark;
    coreBenchmarkFile += "_" + std::to_string(pid);
    coreBenchmarkFile.replace_extension("data");

    if (!std::filesystem::exists(coreBenchmarkFile))
      throw std::domain_error(coreBenchmarkFile.make_preferred().string() + " does not exist in working directory: "
                              + std::filesystem::current_path().make_preferred().string());

    std::shared_ptr<Bus> busPtr;
    if (protocol == "MESI") busPtr = std::make_shared<MESIBus>();
    else
      busPtr = std::make_shared<DragonBus>();

    processor = {std::ifstream(coreBenchmarkFile), pid++, associativity, numBlocks, blockSize};
  }
}

bool System::processorsDone() {
  return std::ranges::all_of(processors, [](const Processor& processor) {
    return processor.blockingInstruction.type == Instruction::InstructionType::DONE;
  });
}

auto System::blockedFor(uint32_t pid) -> uint32_t {
  return bus->cyclesToWaitFor(processors, pid, processors[pid].cache.blockOperation);
}

void System::transitionCacheStates(uint32_t pid) {}

void System::run() {
  while (!processorsDone())
    for (auto& processor : processors) { processor.runOneCycle(); }
}
}// namespace CacheSim
