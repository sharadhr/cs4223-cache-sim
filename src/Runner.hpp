//
// Created by sharadh on 20/10/21.
//

#ifndef CS4223_CACHE_SIM_RUNNER_HPP
#define CS4223_CACHE_SIM_RUNNER_HPP

#include <cstdint>
#include <filesystem>
#include <string>
#include <utility>
#include <vector>

#include "ProcessorPool.hpp"

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

  void printConfig() const;
  void start();
  void printStats();
  static Runner createRunner(int argcount, const char* argv[]);

  explicit Runner(Arguments arguments);
  [[maybe_unused]] explicit Runner(const char* argv[]);

 private:
  CacheSim::ProcessorPool pool;
  static Arguments checkArguments(int argc, const char* argv[]);
};
}// namespace CacheSim

#endif//CS4223_CACHE_SIM_RUNNER_HPP
