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
    processor = {std::ifstream(coreBenchmarkFile), associativity, numBlocks, blockSize};
  }

  if (protocol == "MESI") bus = std::make_unique<MESIBus>(blockSize);
  else bus = std::make_unique<DragonBus>(blockSize);
}

bool System::processorsDone() {
  return std::ranges::all_of(processors, [](const Processor& processor) {
    return processor.blockingInstruction.type == Instruction::InstructionType::DONE;
  });
}

void System::run() {
  while (!processorsDone())
    for (auto& processor : processors) { processor.runOneCycle(); }
}
}// namespace CacheSim
