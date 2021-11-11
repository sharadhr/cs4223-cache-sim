#include "System.hpp"

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <stdexcept>

namespace CacheSim {
System::System(const std::filesystem::path& benchmark, std::string_view protocol, uint8_t associativity,
               uint32_t numBlocks, uint16_t blockSize) {
  uint8_t pid = 0;
  for (auto& processor : processors) {
    auto coreBenchmarkFile = benchmark;
    coreBenchmarkFile += "_" + std::to_string(pid++);
    coreBenchmarkFile.replace_extension("data");

    if (!std::filesystem::exists(coreBenchmarkFile))
      throw std::domain_error(coreBenchmarkFile.make_preferred().string() + " does not exist in working directory: "
                              + std::filesystem::current_path().make_preferred().string());
    processor = {pid, std::ifstream(coreBenchmarkFile), associativity, numBlocks, blockSize};
  }

  if (protocol == "MESI") bus = std::make_shared<MESIBus>(blockSize);
  else bus = std::make_shared<DragonBus>(blockSize);
}

bool System::processorsDone() {
  return std::ranges::all_of(processors, [](const Processor& processor) {
    return processor.blockingInstruction.type == Instruction::InstructionType::DONE;
  });
}

void System::refresh(Processor& processor) {
  auto cache = processor.cache;
  auto caches = getCaches();

  // next instruction
  if (processor.blockedFor == 0 && cache->blockedFor == 0) {
    applyStates(processor);

    processor.fetchInstruction();
    blockProcessor(processor);
  }

  // current instruction
  processor.refresh();
}

void System::blockProcessor(Processor& processor) {
  auto cache = processor.cache;
  auto caches = getCaches();

  switch (processor.blockingInstruction.type) {
    case Instruction::InstructionType::ALU:
      processor.blockedFor = processor.blockingInstruction.value;
      break;
    case Instruction::InstructionType::LD:
      if (cache->has(processor.blockingInstruction.value))
        cache->block(processor.blockingInstruction.value, 1, CacheOp::PR_RD_HIT);
      else {
        if (cache->needsEviction(processor.blockingInstruction.value)) {

          cache->evictFor(processor.blockingInstruction.value);
          return;
        }
        cache->block(processor.blockingInstruction.value,
                     bus->blockedCycles(caches, processor.pid, CacheOp::PR_RD_MISS), CacheOp::PR_RD_MISS);
      }
      break;
    case Instruction::InstructionType::ST:

      if (cache->has(processor.blockingInstruction.value))
        cache->block(processor.blockingInstruction.value, 1, CacheOp::PR_WR_HIT);
      else {
        if (cache->needsEviction(processor.blockingInstruction.value)) {
          processor.blockedFor += 100;
          cache->evictFor(processor.blockingInstruction.value);
          return;
        }
        cache->block(processor.blockingInstruction.value,
                     bus->blockedCycles(caches, processor.pid, CacheOp::PR_WR_MISS), CacheOp::PR_WR_MISS);
      }
      break;
    case Instruction::InstructionType::DONE:
      return;
  }
}

void System::run() {
  while (!processorsDone()) {
    for (auto& processor : processors) refresh(processor);
  }
}

void System::applyStates(Processor& processor) {
  auto caches = getCaches();
  switch (processor.blockingInstruction.type) {
    case Instruction::InstructionType::LD:
    case Instruction::InstructionType::ST:
      if (processor.cache->blockingOperation == CacheOp::PR_WB) {
        bus->transition(caches, processor.pid);
        blockProcessor(processor);
        return;
      }
      bus->transition(caches, processor.pid);
    case Instruction::InstructionType::ALU:
    case Instruction::InstructionType::DONE:
      return;
  }
}

inline std::array<std::shared_ptr<Cache>, 4> System::getCaches() {
  return {processors[0].cache, processors[1].cache, processors[2].cache, processors[3].cache};
}

}// namespace CacheSim
