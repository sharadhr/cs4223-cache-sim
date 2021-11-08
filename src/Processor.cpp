//
// Created by sharadh on 20/10/21.
//

#include "Processor.hpp"

namespace CacheSim {
uint32_t Processor::runOneCycle() {
  int type;
  uint32_t value;

  if (instructionStream >> type >> std::hex >> value >> std::dec) {
    Instruction instruction = {static_cast<Instruction::InstructionType>(type), value};
  } else
    return 1;
  return 0;
}
}// namespace CacheSim
