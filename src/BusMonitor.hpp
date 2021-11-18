#pragma once

#include <cstdint>
namespace CacheSim {
class BusMonitor {
 public:
  uint64_t dataTraffic{};

  std::array<uint64_t, 4> privateAccessCount{};
  std::array<uint64_t, 4> sharedAccessCount{};
  std::array<uint64_t, 4> totalAccessCount{};

  uint64_t invalidationsCount{};
  uint64_t updatesCount{};
  uint64_t writesbackCount{};
};
}// namespace CacheSim
