//
// Created by sharadh on 20/10/21.
//

#include "Processor.hpp"

#include <cstdint>

namespace CacheSim {
CacheSim::Instruction CacheSim::Processor::getNextInstruction() {
  int readType;
  uint32_t value;

  Processor::instructionStream >> readType >> std::hex >> value >> std::dec;

  return {static_cast<Instruction::InstructionType>(readType), value};
}

uint32_t CacheSim::Processor::runNextInstruction() {
  if (!instructionStream.eof()) {
    auto nextInstruction = getNextInstruction();
    switch (nextInstruction.type) {
      case Instruction::LD:
        return readFrom(nextInstruction);
      case Instruction::ST:
        return writeBack(nextInstruction);
      case Instruction::ALU:
        return execute(nextInstruction);
      default:
        throw std::domain_error("");
    }
  }
  return 0;
}

uint32_t CacheSim::Processor::execute(const Instruction& otherInst) {
  pc += otherInst.value;
  return otherInst.value;
}

uint32_t CacheSim::Processor::readFrom(const Instruction& readInst) {
  // auto cycles = cache.get(readInst.value);
  // pc += cycles;
  return 1;
}

uint32_t CacheSim::Processor::writeBack(const Instruction& writeInst) {
  // auto cycles = cache.put(writeInst.value);
  // pc += cycles;
  return 1;
}
}// namespace CacheSim
