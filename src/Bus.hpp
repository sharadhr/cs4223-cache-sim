#ifndef CS4223_CACHE_SIM_BUS_HPP
#define CS4223_CACHE_SIM_BUS_HPP

#include <array>
#include <cstdint>

namespace CacheSim {
class Bus {
 public:
  virtual void handlePrRd(uint8_t, uint32_t) = 0;
  virtual void handlePrdRdMiss(uint8_t, uint32_t) = 0;
  virtual void handlePrWr(uint8_t, uint32_t) = 0;
  virtual void handlePrWrMiss(uint8_t, uint32_t) = 0;

  virtual ~Bus() = default;
};

class MESIBus : public Bus {
 protected:
  void busRd(uint8_t, uint32_t);
  void busRdX(uint8_t, uint32_t);
  void flush(uint8_t, uint32_t);

  bool doOtherCachesContainAddress(uint8_t, uint32_t);

 public:
  void handlePrRd(uint8_t, uint32_t) override;
  void handlePrdRdMiss(uint8_t, uint32_t) override;
  void handlePrWr(uint8_t, uint32_t) override;
  void handlePrWrMiss(uint8_t, uint32_t) override;

  ~MESIBus() override = default;
};

class DragonBus : public Bus {
 protected:
  void busRead(uint8_t, uint32_t);
  void busUpdate(uint8_t, uint32_t);
  void flush(uint8_t, uint32_t);

 public:
  void handlePrRd(uint8_t, uint32_t) override;
  void handlePrdRdMiss(uint8_t, uint32_t) override;
  void handlePrWr(uint8_t, uint32_t) override;
  void handlePrWrMiss(uint8_t, uint32_t) override;

  ~DragonBus() override = default;
};
}// namespace CacheSim

#endif
