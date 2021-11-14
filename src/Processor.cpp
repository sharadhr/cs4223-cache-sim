//
// Created by sharadh on 20/10/21.
//

#include "Processor.hpp"

#include <iostream>

namespace CacheSim {
void Processor::refresh() {
  ++cycleCount;

  switch (blockingInstruction.type) {
    case Type::LD:
    case Type::ST:
      ++monitor.idleCycleCount;
      break;
    case Type::ALU:
      ++monitor.cycleCount;
      break;
    case Type::DONE:
      return;
  }

  if (blockingInstruction.type != Type::DONE) monitor.executionCycleCount++;

  if (blockedFor > 0) --blockedFor;
}

void Processor::fetchInstruction() {
  int type;
  uint32_t value;

  if (instructionStream >> type >> std::hex >> value >> std::dec) {
    blockingInstruction = {static_cast<uint8_t>(type), value};
    switch (blockingInstruction.type) {
      case Instruction::InstructionType::LD:
      case Instruction::InstructionType::ST:
        monitor.loadStoreCount++;
      case Instruction::InstructionType::ALU:
      case Instruction::InstructionType::DONE:
        break;
    }
  } else {
    blockingInstruction = {Type::DONE, 0};
  }
}

void Processor::block(uint32_t blockedCycles) {
  auto cacheOp = getCacheOp();

  switch (cacheOp) {
    case CacheOp::PR_RD_HIT:
    case CacheOp::PR_WR_HIT:
      ++blockedFor;
      cache->setBlocked(blockingInstruction.value, cacheOp);
      break;
    case CacheOp::PR_RD_MISS:
    case CacheOp::PR_WR_MISS:
      if (cache->needsEvictionFor(blockingInstruction.value)) {
        cache->evictFor(blockingInstruction.value);
        return;
      }
      blockedFor = blockedCycles;
      cache->setBlocked(blockingInstruction.value, cacheOp);
      break;
    case CacheOp::PR_WB:
    case CacheOp::PR_NULL:
      if (blockingInstruction.type == Type::ALU) blockedFor = blockingInstruction.value;
      break;
  }
}

void Processor::printData() {
  std::cout << monitor.executionCycleCount << "," << monitor.cycleCount << "," << monitor.idleCycleCount << ","
            << monitor.loadStoreCount << std::endl;
}
}// namespace CacheSim
