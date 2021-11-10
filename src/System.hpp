//
// Created by sharadh on 20/10/21.
//

#ifndef CS4223_CACHE_SIM_SYSTEM_HPP
#define CS4223_CACHE_SIM_SYSTEM_HPP

#include <array>
#include <filesystem>
#include <sstream>

#include "Bus.hpp"
#include "Processor.hpp"

namespace CacheSim {
class System {
 public:
  System() = default;
  System(const std::filesystem::path& benchmark, std::string_view protocol, uint8_t associativity, uint32_t numBlocks,
         uint16_t blockSize);
  void run();

 private:
  std::array<Processor, 4> processors;
  std::unique_ptr<Bus> bus{std::make_unique<MESIBus>()};

  bool processorsDone();
};
}// namespace CacheSim

#endif//CS4223_CACHE_SIM_SYSTEM_HPP
