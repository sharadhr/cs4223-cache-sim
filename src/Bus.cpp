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
  auto ite = invalidBlock.find(blockNum);
  if (ite == invalidBlock.end()) {
    return curTime;
  }

  assert(ite->second >= curTime);
  return ite->second;
}

void Bus::writeBackMem(int processorId, int addr) {
  Cache& cache = processors->at(processorId).cache;
  int blockNum = addr / cache.blockSize;
  assert(invalidBlock[blockNum] == -2);
  invalidBlock[blockNum] = curTime + 100;
  monitor.writebackCount++;
  monitor.trafficData += blockSize;
}

void Bus::checkMem() {
  vector<int> unfreezeBlock;
  for (auto ite : invalidBlock)
    if (ite.second == curTime) {
      unfreezeBlock.push_back(ite.first);
    }

  for (auto block : unfreezeBlock) {
    invalidBlock.erase(block);
  }
}

int Bus::getHeadAddr(int addr) {
  return (addr / blockSize) * blockSize;
}

Bus::~Bus() {
  delete (processors);
}
}// namespace CacheSim
