//
// Created by sharadh on 20/10/21.
//

#ifndef CS4223_CACHE_SIM_PROCESSORPOOL_HPP
#define CS4223_CACHE_SIM_PROCESSORPOOL_HPP

#include <array>
#include <filesystem>
#include <sstream>

#include "Bus.hpp"
#include "Processor.hpp"

namespace CacheSim {
class ProcessorPool {
 public:
  Bus *bus;

  void setup(Protocol protocol, const std::filesystem::path &benchmark, uint16_t associativity, uint16_t numBlocks, uint16_t blockSize);
  void run();

 private:
  std::array<Processor, 4> processors;
  Protocol protocol;
};
}// namespace CacheSim

#endif//CS4223_CACHE_SIM_PROCESSORPOOL_HPP
