#include "Bus.hpp"
#include "Processor.hpp"

namespace CacheSim {

/* DragonBus::DragonBus(std::array<Processor, 4>& processors) : Bus(processors) {} */

void DragonBus::handlePrRd(uint8_t pid, uint32_t address) {}

void DragonBus::handlePrdRdMiss(uint8_t pid, uint32_t address) {}

void DragonBus::handlePrWr(uint8_t pid, uint32_t address) {}

void DragonBus::handlePrWrMiss(uint8_t pid, uint32_t address) {}

void DragonBus::busRead(uint8_t pid, uint32_t address) {}

void DragonBus::busUpdate(uint8_t pid, uint32_t address) {}

void DragonBus::flush(uint8_t pid, uint32_t address) {}
}// namespace CacheSim
