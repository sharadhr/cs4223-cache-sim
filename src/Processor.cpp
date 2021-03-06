//
// Created by sharadh on 20/10/21.
//

#include "Processor.hpp"

#include <iostream>

namespace CacheSim {
void Processor::refresh() {
  switch (blockingInstruction.type) {
    case Type::LD:
    case Type::ST:
      ++monitor.idleCycleCount;
      break;
    case Type::ALU:
      ++monitor.computeCycleCount;
      break;
    case Type::DONE:
      return;
  }
  ++cycleCount;

  if (blockedFor > 0) --blockedFor;
}

void Processor::fetchInstruction() {
  int type;
  uint32_t value;

  if (instructionStream >> type >> std::hex >> value >> std::dec) {
    blockingInstruction = {static_cast<uint8_t>(type), value};
    switch (blockingInstruction.type) {
      case Instruction::InstructionType::LD:
        ++monitor.loadCount;
        break;
      case Instruction::InstructionType::ST:
        ++monitor.storeCount;
        break;
      case Instruction::InstructionType::ALU:
      case Instruction::InstructionType::DONE:
        break;
    }
  } else {
    blockingInstruction = {Type::DONE, 0};
    monitor.cycleCount = cycleCount;
  }

  cache->setCacheOpFor(blockingInstruction.type, blockingInstruction.value);
}

void Processor::block(uint32_t blockedCycles) {
  auto cacheOp = cache->blockingOperation;

  blockedFor = blockedCycles;

  switch (cacheOp) {
    case CacheOp::PR_RD_HIT:
    case CacheOp::PR_WR_HIT: {
      ++monitor.hitCount;
      break;
    }
    case CacheOp::PR_RD_MISS:
    case CacheOp::PR_WR_MISS: {
      ++monitor.missCount;
      break;
    }
    case CacheOp::PR_WB:
    case CacheOp::PR_NULL:
      if (blockingInstruction.type == Type::ALU) blockedFor = blockingInstruction.value;
      break;
  }
}
}// namespace CacheSim
