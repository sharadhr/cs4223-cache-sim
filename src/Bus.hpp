#ifndef CS4223_CACHE_SIM_BUS_HPP
#define CS4223_CACHE_SIM_BUS_HPP

#include <array>
#include <cstdint>

#include "Cache.hpp"

namespace CacheSim {
class Processor;
class CacheState;

class Bus {
 protected:
  uint32_t blockSize{32};

 public:
  virtual uint32_t cyclesToWaitFor(std::array<Processor, 4>&, uint32_t, CacheOp) = 0;
  virtual std::array<CacheState, 4> transition(std::array<CacheState, 4>, uint32_t, CacheOp) = 0;

  virtual ~Bus() = default;
};

class MESIBus : public Bus {
 public:
  uint32_t cyclesToWaitFor(std::array<Processor, 4>&, uint32_t, CacheOp) override;
  std::array<CacheState, 4> transition(std::array<CacheState, 4>, uint32_t, CacheOp) override;

  ~MESIBus() override = default;
};

class DragonBus : public Bus {
 public:
  uint32_t cyclesToWaitFor(std::array<Processor, 4>&, uint32_t, CacheOp) override;
  std::array<CacheState, 4> transition(std::array<CacheState, 4>, uint32_t, CacheOp) override;

  ~DragonBus() override = default;
};
}// namespace CacheSim

#endif
