//
// Created by sharadh on 20/10/21.
//

#ifndef CS4223_CACHE_SIM_RUNNER_HPP
#define CS4223_CACHE_SIM_RUNNER_HPP

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include "ProcessorPool.hpp"

namespace CacheSim {
class Runner {
  struct Arguments {
    std::string protocol{"MESI"};
    std::filesystem::path benchmark{"data/bodytrack"};
    uint32_t cacheSize{4096};
    uint16_t associativity{2};
    uint16_t blockSize{32};
    uint16_t numBlocks{4096 / 32};
  };

 public:
  int argc;
  Arguments args;

  void printConfig() const;
  void start();
  static void printStats();
  static Runner createRunner(int argcount, char* argv[]);

  Runner(int argc, char* argv[]) : argc(argc),
                                   args() {
    if (argc == 6) {
      args = {argv[1],
              std::filesystem::path("data") / argv[2],
              static_cast<uint32_t>(std::stoul(argv[3])),
              static_cast<uint16_t>(std::stoul(argv[4])),
              static_cast<uint16_t>(std::stoul(argv[5])),
              static_cast<uint16_t>(std::stoul(argv[3]) / std::stoul(argv[5]))};
    }
  }

 private:
  CacheSim::ProcessorPool pool;

  static void checkArguments(int argc, char* argv[]);
};
}// namespace CacheSim

#endif//CS4223_CACHE_SIM_RUNNER_HPP
