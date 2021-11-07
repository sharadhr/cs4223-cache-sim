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

bool sortCores(const pair<int, pair<int, int>>& a, const pair<int, pair<int, int>>& b) {
  if (a.first == b.first) {
    return a.second.first < b.second.first;
  }
  return a.first < b.first;
}

void Bus::checkCoreReq() {
  bool busInUse = false;

  vector<pair<int, pair<int, int>>> coreOrder;
  for (int coreID = 0; coreID < (int) processors->size(); coreID++) {
    Processor& core = processors->at(coreID);
    if (core.isDone()) continue;
    if (core.state != Core::FREE) continue;
    coreOrder.push_back(make_pair(core.nextFree,
                                  make_pair(core.monitor.lastBusAccess, coreID)));
  }

  sort(coreOrder.begin(), coreOrder.end(), sortCores);

  for (auto i : coreOrder) {
    int coreID = i.second.second;
    Processor& core = processors->at(coreID);

    assert(!core.isDone());
    assert(core.state == Core::FREE);

    auto instruction = core.instructions.front();
    busInUse = core.issue(instruction, this, busInUse);
  }
}

Bus::~Bus() {
  delete (processors);
}
}// namespace CacheSim
