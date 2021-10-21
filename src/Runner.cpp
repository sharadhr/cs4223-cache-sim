//
// Created by Sharadh on 19/10/2021.
//

#include "Runner.hpp"

#include <iostream>

namespace CacheSim {
void Runner::printConfig() const {
  std::stringstream ss;
  ss << "====================================" << std::endl
     << "Block size: " << args.blockSize << " B" << std::endl
     << "Cache size: " << args.cacheSize << " B";

  if (args.cacheSize > 1024)
    ss << " (" << static_cast<float>(args.cacheSize) / 1024 << " KiB)" << std::endl;
  else
    ss << std::endl;

  ss << "Associativity: ";
  if (args.associativity == 1)
    ss << "direct-mapped" << std::endl;
  else if (args.associativity == args.numBlocks)
    ss << "fully associative" << std::endl;
  else {
    ss << args.associativity << "-way set-associative" << std::endl
       << "Cache blocks: " << args.numBlocks
       << " (" << (args.numBlocks / args.associativity) << " per set)" << std::endl
       << "====================================" << std::endl;
    std::cout << ss.view();
    return;
  }
  ss << "Cache blocks: " << args.numBlocks << std::endl
     << "====================================" << std::endl;

  std::cout << ss.view();
}

void Runner::printStats(/*std::array<CoreMonitor, 4>& coreMons, CacheSim::BusMonitor& busMon*/) {}

void Runner::start() {
  pool.setup(args.benchmark, args.associativity, args.numBlocks, args.blockSize);
  pool.run();
  //
}

Runner Runner::createRunner(int argcount, char** argv) {
  checkArguments(argcount, argv);
  return {argcount, argv};
}

void Runner::checkArguments(int argc, char* argv[]) {
  if (argc < 6) {
    std::cout << "Fewer than 5 arguments were provided. Using defaults." << std::endl;
    return;
  } else if (argc > 6)
    throw std::domain_error("More than 5 arguments were provided. Exiting.");

  std::vector<std::string> arguments(argv, argc + argv);
  if ((arguments[1] != std::string("MESI")) == (arguments[1] != std::string("Dragon")))
    throw std::domain_error(R"(Protocol must be either "MESI" or "Dragon", but ")" + arguments[1] + R"(" was provided instead. Exiting.)");

  if (std::stoi(arguments[3]) % 4 != 0)
    throw std::domain_error("Cache size must be integer multiple of word size (4 B). Provided value was " + arguments[3]);

  if (std::stoi(arguments[5]) % 4 != 0)
    throw std::domain_error("Block size must be integer multiple of word size (4 B). Provided value was " + arguments[5]);
}
}// namespace CacheSim

int main(int argc, char* argv[]) {
  auto runner = CacheSim::Runner::createRunner(argc, argv);
  runner.printConfig();
  runner.start();
  runner.printStats();
  return 0;
}
