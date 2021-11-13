//
// Created by sharadh on 20/10/21.
//

#include "Processor.hpp"

namespace CacheSim {
void Processor::refresh() {
  ++cycleCount;

  switch (blockingInstruction.type) {
    case Type::LD:
    case Type::ST:
      ++monitor.idleCycleCount;
      break;
    case Type::ALU:
      ++monitor.cycleCount;
      break;
    case Type::DONE:
      break;
  }

  if (blockedFor > 0) --blockedFor;
}

void Processor::fetchInstruction() {
  uint8_t type;
  uint32_t value;

  if (instructionStream >> type >> std::hex >> value >> std::dec) blockingInstruction = {type, value};
  else {
    monitor.executionCycleCount = cycleCount;
    blockingInstruction = {Type::DONE, 0};
  }
}

void Processor::block(uint32_t blockedCycles) {
  auto cacheOp = getCacheOp();

  switch (cacheOp) {
    case CacheOp::PR_RD_HIT:
    case CacheOp::PR_WR_HIT:
      ++blockedFor;
      cache->setBlocked(blockingInstruction.value, cacheOp);
      break;
    case CacheOp::PR_RD_MISS:
    case CacheOp::PR_WR_MISS:
      if (cache->needsEvictionFor(blockingInstruction.value)) {
        blockedFor = 100;
        cache->evictFor(blockingInstruction.value);
        return;
      }
      blockedFor = blockedCycles;
      cache->setBlocked(blockingInstruction.value, cacheOp);
      break;
    case CacheOp::PR_WB:
    case CacheOp::PR_NULL:
      if (blockingInstruction.type == Type::ALU) blockedFor = blockingInstruction.value;
      break;
  }
}
}// namespace CacheSim
