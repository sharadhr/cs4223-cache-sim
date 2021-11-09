//
// Created by Sharadh on 19/10/2021.
//

#include "Runner.hpp"

#include <iostream>

namespace CacheSim {
void Runner::printConfig() const {
  std::stringstream ss;
  ss << "====================================" << std::endl << "Cache size: " << args.cacheSize << " B";

  if (args.cacheSize > 1024) ss << " (" << static_cast<float>(args.cacheSize) / 1024 << " KiB)" << std::endl;
  else
    ss << std::endl;

  ss << "Block size: " << args.blockSize << " B" << std::endl;

  ss << "Associativity: ";
  if (args.associativity == 1) ss << "direct-mapped" << std::endl;
  else if (args.associativity == args.numBlocks)
    ss << "fully associative" << std::endl;
  else {
    ss << args.associativity << "-way set-associative" << std::endl
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

void Runner::start() { pool.run(); }

Runner Runner::createRunner(int argcount, char* argv[]) {
  checkArguments(argcount, argv);
  return Runner(argv);
}

void Runner::checkArguments(int argc, char* argv[]) {
  if (argc < 6) {
    std::cout << "Fewer than 5 arguments were provided. Using defaults." << std::endl;
    return;
  } else if (argc > 6)
    throw std::domain_error("More than 5 arguments were provided. Exiting.");

  std::vector<std::string> arguments(argv, argc + argv);
  if ((arguments[1] != "MESI") == (arguments[1] != "Dragon"))
    throw std::domain_error(R"(Protocol must be either "MESI" or "Dragon", but ")" + arguments[1]
                            + R"(" was provided instead.)");

  if (std::stoi(arguments[3]) % 4 != 0)
    throw std::domain_error("Cache size must be integer multiple of word size (4 B). Provided value was "
                            + arguments[3]);

  if (std::stoi(arguments[5]) % 4 != 0)
    throw std::domain_error("Block size must be integer multiple of word size (4 B). Provided value was "
                            + arguments[5]);
}
}// namespace CacheSim

int main(int argc, char* argv[]) {
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
