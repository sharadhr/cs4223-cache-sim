#ifndef CS4223_CACHE_SIM_BUS_HPP
#define CS4223_CACHE_SIM_BUS_HPP

#include <array>
#include <cstdint>

#include "Processor.hpp"

namespace CacheSim {
class Bus {
 public:
  std::array<Processor, 4> processors;
  virtual void handlePrRd(uint32_t processorId, uint32_t address);
  virtual void handlePrdRdMiss(uint32_t processorId, uint32_t address);
  virtual void handlePrWr(uint32_t processorId, uint32_t address);
  virtual void handlePrWrMiss(uint32_t processorId, uint32_t address);
  Bus(std::array<Processor, 4> &processors) : processors(processors) {}
};

class MESIBus : public Bus {
 protected:
  void busRead(uint32_t processorId, uint32_t address);
  void busReadX(uint32_t processorId, uint32_t address);
  void flush(uint32_t processorId, uint32_t address);
  bool doOtherCachesContainAdress(uint32_t processorId, uint32_t address);

 public:
  void handlePrRd(uint32_t processorId, uint32_t address) override;
  void handlePrdRdMiss(uint32_t processorId, uint32_t address) override;
  void handlePrWr(uint32_t processorId, uint32_t address) override;
  void handlePrWrMiss(uint32_t processorId, uint32_t address) override;
  MESIBus(std::array<Processor, 4> &processors) : Bus(processors) {}
};

class DragonBus : public Bus {
 protected:
  void busRead(uint32_t processorId, uint32_t address);
  void busUpdate(uint32_t processorId, uint32_t address);
  void flush(uint32_t processorId, uint32_t address);

 public:
  void handlePrRd(uint32_t processorId, uint32_t address) override;
  void handlePrdRdMiss(uint32_t processorId, uint32_t address) override;
  void handlePrWr(uint32_t processorId, uint32_t address) override;
  void handlePrWrMiss(uint32_t processorId, uint32_t address) override;
  DragonBus(std::array<Processor, 4> &processors) : Bus(processors) {}
};
}// namespace CacheSim

#endif
