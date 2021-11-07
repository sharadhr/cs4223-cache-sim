#include "Bus.hpp"

#include <cassert>
#include <string>
#include <utility>
#include <vector>

using namespace std;

namespace CacheSim {
Bus::Bus(int blockSize, std::array<Processor, 4>* processors) : blockSize(blockSize),
                                                                processors(processors) {
}

int Bus::getMemBlockAvailableTime(int blockNum) {
  auto it = invalidBlocks.find(blockNum);
  if (it == invalidBlocks.end()) {
    return curCycle;
  }

  assert(it->second >= 0);
  return it->second;
}

void Bus::writeBackMem(int addr) {
  int blockNum = addr / blockSize;
  assert(invalidBlocks[blockNum] == -2);
  invalidBlocks[blockNum] = 100;
  monitor.writebackCount++;
  monitor.trafficData += blockSize;
}

void Bus::checkMem() {
  vector<int> unfreezeBlock;
  for (auto it : invalidBlocks)
    if (it.second == 0) {
      unfreezeBlock.push_back(it.first);
    }

  for (auto block : unfreezeBlock) {
    invalidBlocks.erase(block);
  }
}

int Bus::getHeadAddr(int addr) {
  return (addr / blockSize) * blockSize;
}

Bus::~Bus() {
  delete (processors);
}
}// namespace CacheSim
