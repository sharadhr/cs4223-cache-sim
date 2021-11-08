//
// Created by sharadh on 20/10/21.
//

#include "Processor.hpp"

namespace CacheSim {
uint32_t Processor::runOneCycle() {
  if (isDone) {
    return 1;
  }

  pc++;
  if (isBlocked && blockedFor > 0) {
    blockedFor--;
    return 0;
  }

  if (cache.isBlocked && cache.blockedFor > 0) {
    cache.blockedFor--;
    return 0;
  }

  finishCurrentInstruction();
  issueNextInstruction();

  return 0;
}

Instruction Processor::getNextInstruction() {
  int type;
  uint32_t value;
  if (instructionStream >> type >> std::hex >> value >> std::dec) {
    return {static_cast<Instruction::InstructionType>(type), value};
  } else
    return {Instruction::InstructionType::DONE};
}

void Processor::finishCurrentInstruction() {
  if (blockedInstruction.type == Instruction::InstructionType::LD || blockedInstruction.type == Instruction::InstructionType::ST) {
    cache.issueBusTransaction();
  } else {
    // TODO: terminate ALU instruction
  }
}

void Processor::issueNextInstruction() {
  Instruction instruction = getNextInstruction();
  if (instruction.type == Instruction::InstructionType::DONE) {
    isDone = true;
    return;
  }
  // TODO: update cache for mem instructions or block processor for ALU instructions
}

}// namespace CacheSim
