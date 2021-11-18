#pragma once

#include <array>
#include <filesystem>
#include <sstream>

#include "Bus.hpp"
#include "Processor.hpp"

namespace CacheSim {
class System {
 public:
  System() = default;
  System(const std::filesystem::path& benchmark, std::string_view protocol, uint32_t associativity, uint32_t numBlocks,
         uint16_t blockSize);
  void run();

  inline std::array<CoreMonitor, 4> coreMonitors() {
    return {processors[0].monitor, processors[1].monitor, processors[2].monitor, processors[3].monitor};
  }

  inline BusMonitor busMonitor() { return bus->monitor; }

 private:
  std::array<Processor, 4> processors{};
  std::unique_ptr<Bus> bus{};

  bool handleEvictionIfNeeded(Processor& processor);
  bool processorsDone();
  void refresh(Processor& processor);

  inline std::array<std::shared_ptr<Cache>, 4> getCaches() {
    return {processors[0].cache, processors[1].cache, processors[2].cache, processors[3].cache};
  };
};
}// namespace CacheSim
