#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <utility>
#include <vector>

#include "System.hpp"

namespace CacheSim {
class Runner {
  struct Arguments {
    std::string_view protocol{"MESI"};
    std::filesystem::path benchmark{"data/bodytrack"};
    uint32_t cacheSize{4096};
    uint8_t associativity{2};
    uint16_t blockSize{32};
    uint32_t numBlocks{4096 / 32};

    Arguments() = default;
    explicit Arguments(const char* argv[]);
  };

 public:
  Arguments args;

  static Arguments checkArguments(int argc, const char* argv[]);
  void printConfig() const;
  auto start();
  void printStats();
  void writeToFile();

  explicit Runner(Arguments arguments);
  [[maybe_unused]] explicit Runner(const char* argv[]);

 private:
  CacheSim::System simSystem;
};
}// namespace CacheSim
