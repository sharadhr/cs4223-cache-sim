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
  std::array<Processor, 4> processors{};
  std::shared_ptr<Bus> bus{};

  bool processorsDone();
  void refresh(Processor& processor);
  void blockProcessor(Processor& processor);
  void applyStates(Processor& processor);
  inline std::array<std::shared_ptr<Cache>, 4> getCaches();
};
}// namespace CacheSim

#endif//CS4223_CACHE_SIM_SYSTEM_HPP
