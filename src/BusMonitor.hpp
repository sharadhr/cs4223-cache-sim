#pragma once

#include <cstdint>
namespace CacheSim {
class BusMonitor {
 public:
  // message counts
  uint64_t busRdCount;
  uint64_t busRdXCount;
  uint64_t busUpdCount;
  uint64_t totalTransactionCount;

  uint64_t busRdTraffic;
  uint64_t busRdXTraffic;
  uint64_t busUpdTraffic;

  uint64_t trafficData;

  uint64_t privateAccessCount;
  uint64_t sharedAccessCount;

  uint64_t numOfInvalidationsOrUpdates;
};
}// namespace CacheSim
