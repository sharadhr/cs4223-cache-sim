//
// Created by sharadh on 20/10/21.
//

#include "Processor.hpp"

namespace CacheSim {
void Processor::runOneCycle() {
  ++cycleCount;

  if (isBlocked && blockedFor > 0) --blockedFor;
  if (cache.isBlocked) cache.update();
}

Instruction Processor::getNextInstruction() {
  int type;
  uint32_t value;

  if (instructionStream >> type >> std::hex >> value >> std::dec) return {Instruction::InstructionType(type), value};
  else {
    monitor.executionCycleCount = cycleCount;
    return {Instruction::InstructionType::DONE, 0};
  }
}

void Processor::issueNextInstruction() {
  blockingInstruction = getNextInstruction();

  switch (blockingInstruction.type) {
    case Instruction::InstructionType::LD:
      isBlocked = true;
      // set blockedFor with return value from Cache API
      // increment monitor.idleCycleCount with blockedFor value
      ++monitor.loadStoreCount;
      break;
    case Instruction::InstructionType::ST:
      isBlocked = true;
      // set blockedFor with return value from Cache API
      // increment monitor.idleCycleCount with blockedFor value
      ++monitor.loadStoreCount;
      break;
    case Instruction::InstructionType::ALU:
      isBlocked = true;
      blockedFor = blockingInstruction.value;
      monitor.computeCycleCount += blockedFor;
      break;
    case Instruction::InstructionType::DONE:
      isBlocked = false;
      break;
  }
}

}// namespace CacheSim
