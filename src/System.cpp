#include "System.hpp"

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <ranges>
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
  else if (protocol == "MOESI") bus = std::make_shared<MOESIBus>(blockSize);
  else bus = std::make_shared<DragonBus>(blockSize);
}

bool System::processorsDone() {
  return std::ranges::all_of(
      processors, [](const Processor& processor) { return processor.blockingInstruction.type == Type::DONE; });
}

void System::refresh(Processor& processor) {
  // next instruction
  if (!processor.isBlocked()) {
    if (processor.cache->blockingOperation != CacheOp::PR_NULL)
      bus->transition(getCaches(), processor.pid, processor.blockingInstruction.value);

    processor.fetchInstruction();
    bool needsWriteback = handleEvictionIfNeeded(processor);

    if (!needsWriteback) {
      auto blockingCycles = bus->getBlockedCycles(getCaches(), processor.cache->blockingOperation,
                                                  processor.blockingInstruction.value, processor.pid);
      processor.block(blockingCycles);
    }
  }

  // current instruction
  processor.refresh();
}

bool System::handleEvictionIfNeeded(Processor& processor) {
  switch (processor.cache->blockingOperation) {
    case CacheOp::PR_RD_MISS:
    case CacheOp::PR_WR_MISS: {
      if (processor.cache->needsEvictionFor(processor.blockingInstruction.value)) {
        bool needsWriteBack = processor.cache->needsWriteBack(processor.blockingInstruction.value);
        bus->handleEviction(getCaches(), processor.pid,
                            processor.cache->evictedBlock(processor.blockingInstruction.value));

        if (needsWriteBack) {
          processor.block(100);
          return true;
        }
      }
      break;
    }
    default:
      break;
  }
  return false;
}

void System::run() {
  while (!processorsDone()) std::ranges::for_each(processors, [&](Processor& processor) { refresh(processor); });

  printPostRunStats();
}

void System::printPostRunStats() {
  std::cout << "executionCycles,computeCycles,idleCycles,loadStoreCount,missCount,hitCount" << std::endl;
  for (auto i = 0; i < 4; i++) { processors[i].printData(); }
  std::cout << "totalTime,"
            << std::ranges::max_element(processors,
                                        [](const Processor& p1, const Processor& p2) {
                                          return p1.monitor.executionCycleCount < p2.monitor.executionCycleCount;
                                        })
                   ->monitor.executionCycleCount
            << std::endl;

  std::cout << "privateDataAccessCount,sharedDataAccessCount,numberOfInvalidationsOrUpdates,trafficData" << std::endl;
  std::cout << bus->monitor.privateAccessCount << "," << bus->monitor.sharedAccessCount << ","
            << bus->monitor.numOfInvalidationsOrUpdates << "," << bus->monitor.trafficData << std::endl;
}
}// namespace CacheSim
