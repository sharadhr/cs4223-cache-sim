//
// Created by sharadh on 20/10/21.
//

#include "Processor.hpp"

namespace CacheSim {
void Processor::run(int cycles) {
  if (isDone()) {
    return;
  }
  pc += cycles;
}

bool Processor::isDone() {
  return state == Core::FREE && instructions.empty();
}

void Processor::loadInstructions() {
  int type;
  uint32_t value;
  while (true) {
    if (instructionStream >> type >> std::hex >> value >> std::dec) {
      instructions.push_back({static_cast<Instruction::InstructionType>(type), value});
    } else
      return;
  }
}
}// namespace CacheSim
