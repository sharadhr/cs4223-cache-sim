//
// Created by sharadh on 20/10/21.
//

#include "Processor.hpp"

#include "Bus.hpp"

namespace CacheSim {
void Processor::run(int cycles) {
  if (isDone()) {
    return;
  }
  pc += cycles;
}

bool isMemoryInstruction(Instruction instruction) {
  return instruction.type == Instruction::InstructionType::LD || instruction.type == Instruction::InstructionType::ST;
}

void Core::setBusy(int nextFree) {
  this->state = BUSY;
  this->nextFree = nextFree;
}

bool Processor::issue(Instruction instruction, Bus *bus, bool busInUse) {
  auto instructionType = instruction.type;
  auto addr = instruction.value;

  if (isMemoryInstruction(instruction)) {
    if (cache.has(addr)) {
      if (instructionType == Instruction::InstructionType::LD) {
        instructions.pop_front();
        bus->readHit(pid, addr);
      }
      if (instructionType == Instruction::InstructionType::ST) {
        if (bus->blocksUnderRequest.find(bus->getHeadAddr(addr)) == bus->blocksUnderRequest.end()) {
          bus->writeHit(pid, addr);
          instructions.pop_front();
        } else {
          monitor.idleCycles++;
          return busInUse;
        }
      }
    } else {
      if (busInUse || bus->blocksUnderRequest.find(bus->getHeadAddr(addr)) != bus->blocksUnderRequest.end()) {
        monitor.idleCycles++;
        return busInUse;
      }

      busInUse = true;
      instructions.pop_front();

      monitor.cacheMissCount++;

      bus->blocksUnderRequest[bus->getHeadAddr(addr)] = pid;
      monitor.lastBusAccess = bus->curCycle;

      if (instructionType == Instruction::InstructionType::LD) {
        bus->readMiss(pid, addr);
      }

      if (instructionType == Instruction::InstructionType::ST) {
        bus->writeMiss(pid, addr);
      }
    }

    if (cache.get(addr).state == CacheLine::EXCLUSIVE || cache.get(addr).state == CacheLine::DIRTY) {
      monitor.privateAccessCount++;
    }

    if (instructionType == Instruction::InstructionType::LD) {
      monitor.loadCount++;
    } else {
      monitor.storeCount++;
    }

    int nextFree = std::max(bus->curCycle, cache.get(addr).validFrom) + 1;
    monitor.idleCycles = nextFree - bus->curCycle;
    setBusy(nextFree);
  }

  if (instructionType == Instruction::InstructionType::ALU) {
    instructions.pop_front();

    int computeTime = instruction.value;
    setBusy(bus->curCycle + computeTime);
    monitor.compCycles += computeTime;
  }
  return busInUse;
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
