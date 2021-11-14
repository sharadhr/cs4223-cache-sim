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
  System(const std::filesystem::path& benchmark, std::string_view protocol, uint8_t associativity, uint32_t numBlocks,
         uint16_t blockSize);
  void run();

 private:
  std::array<Processor, 4> processors{};
  std::shared_ptr<Bus> bus{};

  void printPostRunStats();

  bool processorsDone();
  void refresh(Processor& processor);
  inline std::array<std::shared_ptr<Cache>, 4> getCaches() {
    return {processors[0].cache, processors[1].cache, processors[2].cache, processors[3].cache};
  };
};
}// namespace CacheSim
