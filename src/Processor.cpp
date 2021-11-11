//
// Created by sharadh on 20/10/21.
//

#include "Processor.hpp"

namespace CacheSim {
void Processor::refresh() {
  ++cycleCount;

  switch (blockingInstruction.type) {
    case Instruction::InstructionType::LD:
    case Instruction::InstructionType::ST:
      ++monitor.idleCycleCount;
      break;
    case Instruction::InstructionType::ALU:
      ++monitor.executionCycleCount;
      break;
    case Instruction::InstructionType::DONE:
      break;
  }

  if (blockedFor > 0 && cache->blockedFor > 0) {
    --blockedFor;
    cache->refresh();
  }
}

Instruction Processor::fetchInstruction() {
  int type;
  uint32_t value;

  if (instructionStream >> type >> std::hex >> value >> std::dec) {
    blockingInstruction = {Instruction::InstructionType(type), value};
    return blockingInstruction;
  } else {
    monitor.executionCycleCount = cycleCount;
    blockingInstruction = {Instruction::InstructionType::DONE, 0};
    return blockingInstruction;
  }
}
}// namespace CacheSim
