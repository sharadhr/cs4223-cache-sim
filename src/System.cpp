#include "System.hpp"

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <stdexcept>

namespace CacheSim {
using Type = CacheSim::Instruction::InstructionType;

System::System(const std::filesystem::path& benchmark, std::string_view protocol, uint8_t associativity,
               uint32_t numBlocks, uint16_t blockSize) {
  uint8_t pid = 0;
  for (auto& processor : processors) {
    auto coreBenchmarkFile = benchmark;
    coreBenchmarkFile += "_" + std::to_string(pid);
    coreBenchmarkFile.replace_extension("data");

    if (!std::filesystem::exists(coreBenchmarkFile))
      throw std::domain_error(coreBenchmarkFile.make_preferred().string() + " does not exist in working directory: "
                              + std::filesystem::current_path().make_preferred().string());
    processor = {pid++, std::ifstream(coreBenchmarkFile), associativity, numBlocks, blockSize};
  }

  if (protocol == "MESI") bus = std::make_shared<MESIBus>(blockSize);
  else
    bus = std::make_shared<DragonBus>(blockSize);
}

bool System::processorsDone() {
  return std::ranges::all_of(processors,
                             [](const auto& processor) { return processor.blockingInstruction.type == Type::DONE; });
}

void System::refresh(Processor& processor) {
  // next instruction
  if (!processor.isBlocked()) {
    applyStates(processor);

    if (processor.getCacheOp() != CacheOp::PR_WB) processor.fetchInstruction();
    auto blockingCycles =
        bus->getBlockedCycles(getCaches(), processor.getCacheOp(), processor.blockingInstruction.value);
    processor.block(blockingCycles);
  }

  // current instruction
  processor.refresh();
}

void System::run() {
  while (!processorsDone())
    std::for_each(processors.begin(), processors.end(), [&](Processor& processor) { refresh(processor); });
}

void System::applyStates(Processor& processor) {
  auto cacheOp = processor.getCacheOp();
  if (cacheOp != CacheOp::PR_NULL) bus->transition(getCaches(), processor.pid, processor.blockingInstruction.value);
}
}// namespace CacheSim
