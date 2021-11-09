//
// Created by Sharadh on 19/10/2021.
//

#include "Runner.hpp"

#include <iostream>

namespace CacheSim {
Runner::Arguments::Arguments(const char* argv[]) :
    protocol(argv[1]),
    benchmark(std::filesystem::path("data") / argv[2]),
    cacheSize(static_cast<uint32_t>(std::stoul(argv[3]))),
    associativity(static_cast<uint8_t>(std::stoul(argv[4]))),
    blockSize(static_cast<uint16_t>(std::stoul(argv[5]))),
    numBlocks(cacheSize / blockSize) {}

Runner::Arguments Runner::checkArguments(int argc, const char* argv[]) {
  if (argc < 6) {
    std::cout << "Fewer than 5 arguments were provided. Using defaults." << std::endl;
    return {};
  } else if (argc > 6) throw std::domain_error("More than 5 arguments were provided. Exiting.");

  std::vector<std::string> argVec(argv, argv + argc);
  Arguments arguments(argv);
  if (arguments.protocol != "MESI" && arguments.protocol != "Dragon")
    throw std::domain_error(R"(Protocol must be either "MESI" or "Dragon", but ")" + argVec[1]
                            + R"(" was provided instead.)");

  if (arguments.blockSize % 4 != 0)
    throw std::domain_error("Block size must be integer multiple of word size (4 B). Provided value was "
                            + std::to_string(arguments.blockSize));

  if (arguments.cacheSize % arguments.blockSize)
    throw std::domain_error("Cache size must be integer multiple of block size (" + argVec[5] + " B). Provided "
                            "value was " + argVec[3]);

  if (arguments.numBlocks % arguments.associativity != 0)
    throw std::domain_error(
        "Associativity does not evenly divide number of blocks. Provided values were: associativity: " + argVec[4]
        + " Number of blocks: " + std::to_string(arguments.numBlocks));

  return arguments;
}

Runner::Runner(Runner::Arguments arguments) : args(std::move(arguments)) {
  pool.setup(args.benchmark, args.protocol, args.associativity, args.numBlocks, args.blockSize);
}

[[maybe_unused]] Runner::Runner(const char* argv[]) : args(argv) {
  pool.setup(args.benchmark, args.protocol, args.associativity, args.numBlocks, args.blockSize);
}

void Runner::printConfig() const {
  std::stringstream ss;
  ss << "====================================" << std::endl << "Cache size: " << args.cacheSize << " B";

  if (args.cacheSize > (1 << 10) && args.cacheSize < (1 << 20))
    ss << " (" << std::setprecision(4) << static_cast<double>(args.cacheSize) / static_cast<double>(1 << 10) << " KiB)"
       << std::endl;
  else if (args.cacheSize > (1 << 20))
    ss << " (" << std::setprecision(4) << static_cast<double>(args.cacheSize) / static_cast<double>(1 << 20) << " MiB)"
       << std::endl;
  else ss << std::endl;

  ss << "Block size: " << args.blockSize << " B" << std::endl;

  ss << "Associativity: ";
  if (args.associativity == 1) ss << "direct-mapped" << std::endl;
  else if (args.associativity == args.numBlocks) ss << "fully associative" << std::endl;
  else {
    ss << std::to_string(args.associativity) << "-way set-associative" << std::endl
       << "Cache blocks: " << args.numBlocks << " (" << (args.numBlocks / args.associativity) << " per set)"
       << std::endl
       << "====================================" << std::endl;
    std::cout << ss.view();
    return;
  }
  ss << "Cache blocks: " << args.numBlocks << std::endl << "====================================";

  std::cout << ss.view() << std::endl;
}

void Runner::printStats() {
  // collect monitors from pool.processors, print stats to file/stdout
  // collect bus monitor, print stats to file/stdout
}

auto Runner::start() -> void { pool.run(); }

auto Runner::createRunner(int argcount, const char* argv[]) -> Runner { return Runner(checkArguments(argcount, argv)); }
}// namespace CacheSim

int main(int argc, const char* argv[]) {
  try {
    auto runner = CacheSim::Runner::createRunner(argc, argv);
    runner.printConfig();
    runner.start();
    runner.printStats();
    return 0;
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
}
