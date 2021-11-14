#pragma once

#include <cstdint>
namespace CacheSim {
class BusMonitor {
 public:
  uint64_t busRdCount;
  uint64_t busRdXCount;
  uint64_t busUpdCount;

  uint64_t privateAccessCount;
  uint64_t sharedAccessCount;
};
}// namespace CacheSim
