//
// Created by sharadh on 20/10/21.
//

#ifndef CS4223_CACHE_SIM_PROCESSORPOOL_HPP
#define CS4223_CACHE_SIM_PROCESSORPOOL_HPP

#include <array>
#include <filesystem>
#include <sstream>

#include "CoreMonitor.hpp"
#include "Processor.hpp"

namespace CacheSim {
class ProcessorPool {
 public:
  ProcessorPool() = default;
  void run();
  void setup(const std::filesystem::path& benchmark, std::string_view protocol, uint8_t associativity,
             uint32_t numBlocks, uint16_t blockSize);
  bool processorsDone();

 private:
  std::array<Processor, 4> processors;

  friend class Runner;
};
}// namespace CacheSim

#endif//CS4223_CACHE_SIM_PROCESSORPOOL_HPP
