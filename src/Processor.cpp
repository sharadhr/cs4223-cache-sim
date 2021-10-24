//
// Created by sharadh on 20/10/21.
//

#include "Processor.hpp"

namespace CacheSim {
uint32_t Processor::runNextInstruction() {
  int type; uint32_t value;
  if (instructionStream >> type >> std::hex >> value >> std::dec) {

    Instruction nextInstruction = {static_cast<Instruction::InstructionType>(type), value};

    switch (nextInstruction.type) {
      case Instruction::InstructionType::LD:
        return readFrom(nextInstruction);
      case Instruction::InstructionType::ST:
        return writeBack(nextInstruction);
      case Instruction::InstructionType::ALU:
        return execute(nextInstruction);
      default:
        return 0;
    }
  }
  return 0;
}

uint16_t Processor::execute(const Instruction& otherInst) {
  pc += 1;
  return 1;
  // return static_cast<uint16_t>(otherInst.value);
}

uint16_t Processor::readFrom(const Instruction& readInst) {
  // auto cycles = cache.get(readInst.value);
  // pc += cycles;
  pc += 1;
  return 1;
}

uint16_t Processor::writeBack(const Instruction& writeInst) {
  // auto cycles = cache.put(writeInst.value);
  // pc += cycles;
  pc += 1;
  return 1;
}
}// namespace CacheSim