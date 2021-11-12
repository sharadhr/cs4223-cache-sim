#include "Cache.hpp"

#include <algorithm>


namespace CacheSim {
void Cache::lruShuffle(uint32_t address) {
  if (!contains(address)) return;
  auto blockNum = address / blockSize;
  auto setNum = blockNum % numBlocks;
  auto blockIndex = getBlockIndex(address);

  std::vector<CacheLine>& currentSet = store[setNum];

  auto line = currentSet[blockIndex];
  currentSet.erase(currentSet.begin() + blockIndex);
  currentSet.push_back(line);
}

uint32_t Cache::getBlockIndex(uint32_t address) {
  auto blockNum = address / blockSize;
  auto setNum = blockNum % numBlocks;

  std::vector<CacheLine> currentSet = store[setNum];

  for (uint32_t i = 0; i < currentSet.size(); i++) {
    const auto& line = currentSet[i];
    if (line.blockNum == blockNum && line.state != CacheLine::CacheState::INVALID) return i;
  }
  return UINT32_MAX;
}

bool Cache::contains(uint32_t address) {
  uint32_t blockNum = address / blockSize;
  uint32_t setNum = blockNum % numBlocks;

  std::vector<CacheLine> currentSet = store[setNum];

  return std::ranges::all_of(currentSet, [&blockNum](const CacheLine& line) {
    return line.blockNum == blockNum && line.state != CacheLine::CacheState::INVALID;
  });
}

void Cache::setBlock(uint32_t address, uint32_t blockedCycles, CacheOp operation) {
  blockedOnAddress = address;
  blockedFor = blockedCycles;
  blockingOperation = operation;
}

void Cache::refresh() {
  if (blockedFor > 0) --blockedFor;
}

bool Cache::needsEvictionFor(uint32_t incomingAddress) {
  uint32_t setNum = incomingAddress / blockSize;
  return store[setNum][0].state != CacheLine::CacheState::INVALID;
}

void Cache::evictAndBlock(uint32_t incomingAddress) {
  uint32_t setNum = incomingAddress / blockSize;
  if (store[setNum][0].dirty) setBlock(store[setNum][0].blockNum, 100, CacheOp::PR_WB);
  else setBlock(store[setNum][0].blockNum, 0, CacheOp::PR_WB);
}
CacheOp Cache::getCacheOpFor(Instruction::InstructionType type, uint32_t address) {
  switch (type) {
    case Instruction::InstructionType::LD:
      return contains(address) ? CacheOp::PR_RD_HIT : CacheOp::PR_RD_MISS;
    case Instruction::InstructionType::ST:
      return contains(address) ? CacheOp::PR_WR_HIT : CacheOp::PR_WR_MISS;
    case Instruction::InstructionType::ALU:
    case Instruction::InstructionType::DONE:
      return CacheOp::PR_NULL;
  }
}
}// namespace CacheSim
