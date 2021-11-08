//
// Created by sharadh on 20/10/21.
//

#include "Processor.hpp"

namespace CacheSim {
void Processor::runOneCycle() {
  pc++;
  if (isBlocked && blockedFor > 0) {
    blockedFor--;
  }

  if (cache.isBlocked) {
    cache.update();
  }

  issueNextInstruction();
}

Instruction Processor::getNextInstruction() {
  int type;
  uint32_t value;
  if (instructionStream >> type >> std::hex >> value >> std::dec) {
    return {static_cast<Instruction::InstructionType>(type), value};
  } else
    return {Instruction::InstructionType::DONE};
}

void Processor::issueNextInstruction() {
  blockedInstruction = getNextInstruction();
  if (blockedInstruction.type == Instruction::InstructionType::DONE) {
    return;
  }
  // TODO: update cache for mem instructions or block processor for ALU instructions
}

}// namespace CacheSim
