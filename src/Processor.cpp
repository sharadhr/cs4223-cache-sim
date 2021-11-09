//
// Created by sharadh on 20/10/21.
//

#include "Processor.hpp"

namespace CacheSim {
void Processor::runOneCycle() {
  ++cycleCounter;

  if (isBlocked && blockedFor > 0) --blockedFor;
  if (cache.isBlocked) cache.update();
  issueNextInstruction();
}

Instruction Processor::getNextInstruction() {
  int type;
  uint32_t value;

  if (instructionStream >> type >> std::hex >> value >> std::dec) return {Instruction::InstructionType(type), value};
  else {
    monitor.computeCycles[pid] = cycleCounter;
    return {Instruction::InstructionType::DONE};
  }
}

void Processor::issueNextInstruction() {
  blockingInstruction = getNextInstruction();

  switch (blockingInstruction.type) {
    case Instruction::InstructionType::LD:
      isBlocked = true;
      // set blockedFor with return value from Cache API
      break;
    case Instruction::InstructionType::ST:
      isBlocked = true;
      // set blockedFor with return value from Cache API
      break;
    case Instruction::InstructionType::ALU:
      isBlocked = true;
      blockedFor = blockingInstruction.value;
      monitor.executionCycles[pid] += blockedFor;
      break;
    case Instruction::InstructionType::DONE:
      isBlocked = false;
      break;
  }
}

}// namespace CacheSim
