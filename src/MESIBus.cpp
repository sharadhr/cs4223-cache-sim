#include "Bus.hpp"

namespace CacheSim {
bool MESIBus::doOtherCachesContainAdress(uint32_t processorId, uint32_t address) {
}

void MESIBus::handlePrRd(uint32_t processorId, uint32_t addr) {
  processors[processorId].cache.blockAfterPrRdHit(addr);
}

void MESIBus::handlePrdRdMiss(uint32_t processorId, uint32_t addr) {
}

void MESIBus::handlePrWr(uint32_t processorId, uint32_t addr) {
}

void MESIBus::handlePrWrMiss(uint32_t processorId, uint32_t addr) {
}

void MESIBus::busRead(uint32_t processorId, uint32_t addr) {
}

void MESIBus::busReadX(uint32_t processorId, uint32_t addr) {
}

void MESIBus::flush(uint32_t processorId, uint32_t addr) {
}
}// namespace CacheSim
