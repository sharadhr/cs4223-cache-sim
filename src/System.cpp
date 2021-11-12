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
  else bus = std::make_shared<DragonBus>(blockSize);
}

bool System::processorsDone() {
  return std::ranges::all_of(
      processors, [](const Processor& processor) { return processor.blockingInstruction.type == Type::DONE; });
}

void System::refresh(Processor& processor) {
  auto cache = processor.cache;

  // next instruction
  if (processor.blockedFor == 0 && cache->blockedFor == 0) {
    applyStates(processor);

    processor.fetchInstruction();
    auto blockingCycles = bus->getBlockedCycles(getCaches(), processor.pid, processor.getCacheOp());
    processor.block(blockingCycles);
  }

  // current instruction
  processor.refresh();
}

void System::run() {
  while (!processorsDone()) {
    for (auto& processor : processors) refresh(processor);
  }
}

void System::applyStates(Processor& processor) {
  switch (processor.blockingInstruction.type) {
    case Type::LD:
    case Type::ST:
      if (processor.getCacheOp() == CacheOp::PR_WB) {
        bus->transition(getCaches(), processor.pid);
        processor.block(bus->getBlockedCycles(getCaches(), processor.pid, processor.getCacheOp()));
        return;
      }
      bus->transition(getCaches(), processor.pid);
    case Type::ALU:
    case Type::DONE:
      return;
  }
}

inline std::array<std::shared_ptr<Cache>, 4> System::getCaches() {
  return {processors[0].cache, processors[1].cache, processors[2].cache, processors[3].cache};
}
}// namespace CacheSim
