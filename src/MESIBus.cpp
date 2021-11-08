#include "Bus.hpp"

namespace CacheSim {
bool MESIBus::doOtherCachesContainAddress(uint8_t pid, uint32_t address) { return false; }

void MESIBus::handlePrRd(uint8_t pid, uint32_t address) { /*processor.cache.blockAfterPrRdHit(address);*/ }

void MESIBus::handlePrdRdMiss(uint8_t pid, uint32_t address) {}

void MESIBus::handlePrWr(uint8_t pid, uint32_t address) {}

void MESIBus::handlePrWrMiss(uint8_t pid, uint32_t address) {}

void MESIBus::busRd(uint8_t pid, uint32_t address) {}

void MESIBus::busRdX(uint8_t pid, uint32_t address) {}

void MESIBus::flush(uint8_t pid, uint32_t address) {}
}// namespace CacheSim
