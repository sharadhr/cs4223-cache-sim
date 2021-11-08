#include "Bus.hpp"

namespace CacheSim {
void DragonBus::handlePrRd(uint8_t pid, uint32_t address) {}

void DragonBus::handlePrdRdMiss(uint8_t pid, uint32_t address) {}

void DragonBus::handlePrWr(uint8_t pid, uint32_t address) {}

void DragonBus::handlePrWrMiss(uint8_t pid, uint32_t address) {}

void DragonBus::busRead(uint8_t pid, uint32_t address) {}

void DragonBus::busUpdate(uint8_t pid, uint32_t address) {}

void DragonBus::flush(uint8_t pid, uint32_t address) {}
}// namespace CacheSim
