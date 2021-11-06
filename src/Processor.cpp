//
// Created by sharadh on 20/10/21.
//

#include "Processor.hpp"

namespace CacheSim {
uint32_t Processor::runOneCycle() {

  auto exit = false;

  if (exit) {
    int type;
    uint32_t value;

    if (instructionStream >> type >> std::hex >> value >> std::dec) {
      /* pipelineState.instruction = {static_cast<Instruction::InstructionType>(type), value}; */
    } else
      return 1;
  }
  return 0;
}
}// namespace CacheSim
