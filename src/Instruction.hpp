#pragma once

namespace CacheSim {
struct Instruction {
  enum class InstructionType { LD, ST, ALU, DONE };
  InstructionType type{InstructionType::ALU};
  uint32_t value{};

  Instruction() : type(InstructionType::ALU), value(0){};
  Instruction(uint8_t typeNum, uint32_t value) : type(InstructionType(typeNum)), value(value){};
  Instruction(InstructionType type, uint32_t value) : type(type), value(value){};
};

using Type = Instruction::InstructionType;
}// namespace CacheSim