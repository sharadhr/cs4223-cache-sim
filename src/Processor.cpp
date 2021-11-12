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
      ++monitor.computeCycleCount;
      break;
    case Type::DONE:
      break;
  }

  if (blockedFor > 0 && cache->blockedFor > 0) {
    --blockedFor;
    cache->refresh();
  }
}

void Processor::fetchInstruction() {
  int type;
  uint32_t value;

  if (instructionStream >> type >> std::hex >> value >> std::dec) blockingInstruction = {Type(type), value};
  else {
    monitor.executionCycleCount = cycleCount;
    blockingInstruction = {Type::DONE, 0};
  }
}

inline CacheOp Processor::getCacheOp() const {
  return cache->getCacheOpFor(blockingInstruction.type, blockingInstruction.value);
}

void Processor::block(uint32_t blockedCycles) {
  auto cacheOp = getCacheOp();
  switch (cacheOp) {
    case CacheOp::PR_RD_HIT:
    case CacheOp::PR_WR_HIT:
      cache->setBlock(blockingInstruction.value, 1, cacheOp);
      break;
    case CacheOp::PR_RD_MISS:
    case CacheOp::PR_WR_MISS:
      if (cache->needsEvictionFor(blockingInstruction.value)) {
        blockedFor += 100;
        cache->evictAndBlock(blockingInstruction.value);
      }
      cache->setBlock(blockingInstruction.value, blockedCycles, cacheOp);
      break;
    case CacheOp::PR_WB:
    case CacheOp::PR_NULL:
      break;

      // case Type::LD:
      //   auto cacheOp = getCacheOp();
      //   if (cache->contains(blockingInstruction.value)) cache->setBlock(blockingInstruction.value, 1, CacheOp::PR_RD_HIT);
      //   else if (cache->needsEvictionFor(blockingInstruction.value)) {
      //     blockedFor += 100;
      //     cache->evictAndBlock(blockingInstruction.value);
      //   } else cache->setBlock(blockingInstruction.value, blockedCycles, CacheOp::PR_RD_MISS);
      //   break;
      // case Type::ST:
      //   if (cache->contains(blockingInstruction.value)) cache->setBlock(blockingInstruction.value, 1, CacheOp::PR_WR_HIT);
      //   else if (cache->needsEvictionFor(blockingInstruction.value)) {
      //     blockedFor += 100;
      //     cache->evictAndBlock(blockingInstruction.value);
      //   } else cache->setBlock(blockingInstruction.value, blockedCycles, CacheOp::PR_WR_MISS);
      //   break;
      // case Type::ALU:
      //   blockedFor += blockingInstruction.value;
      // case Type::DONE:
      //   break;
  }
}
}// namespace CacheSim
