#include "Runner.hpp"

#include <iostream>

namespace CacheSim {

static constexpr std::string_view pid_s{"pid"};
static constexpr std::string_view exec_s{"execution_cycles"};
static constexpr std::string_view comp_s{"compute_cycles"};
static constexpr std::string_view idle_s{"idle_cycles"};
static constexpr std::string_view load_s{"loads"};
static constexpr std::string_view stor_s{"stores"};
static constexpr std::string_view hitc_s{"hit_count"};
static constexpr std::string_view misc_s{"miss_count"};
static constexpr std::string_view misr_s{"miss_rate"};
static constexpr std::string_view prvt_s{"private"};
static constexpr std::string_view shrd_s{"shared"};
static constexpr std::string_view ptrt_s{"private_rate"};

Runner::Arguments::Arguments(const char* argv[]) :
    protocol(argv[1]),
    benchmark(std::filesystem::path("data") / argv[2]),
    cacheSize(static_cast<uint32_t>(std::stoul(argv[3]))),
    associativity(static_cast<uint8_t>(std::stoul(argv[4]))),
    blockSize(static_cast<uint16_t>(std::stoul(argv[5]))),
    numBlocks(cacheSize / blockSize) {}

Runner::Runner(Runner::Arguments arguments) :
    args(std::move(arguments)),
    simSystem(args.benchmark, args.protocol, args.associativity, args.numBlocks, args.blockSize) {}

[[maybe_unused]] Runner::Runner(const char* argv[]) :
    args(argv),
    simSystem(args.benchmark, args.protocol, args.associativity, args.numBlocks, args.blockSize) {}

Runner::Arguments Runner::checkArguments(int argc, const char* argv[]) {
  if (argc < 6) {
    std::cout << "Fewer than 5 arguments were provided. Using defaults." << std::endl;
    return {};
  } else if (argc > 6) throw std::domain_error("More than 5 arguments were provided. Exiting.");

  std::vector<std::string> argVec(argv, argv + argc);
  Arguments arguments(argv);
  if (arguments.protocol != "MESI" && arguments.protocol != "MOESI" && arguments.protocol != "Dragon")
    throw std::domain_error(R"(Protocol must be either "MESI", "MOESI" or "Dragon", but ")" + argVec[1]
                            + R"(" was encountered instead.)");

  if (arguments.blockSize % 4 != 0)
    throw std::domain_error("Block size must be integer multiple of word size (4 B). Encountered value was "
                            + std::to_string(arguments.blockSize));

  if (arguments.cacheSize % arguments.blockSize)
    throw std::domain_error("Cache size must be integer multiple of setBlock size (" + argVec[5] + " B). Encountered "
                            "value was " + argVec[3]);

  if (arguments.numBlocks % arguments.associativity != 0)
    throw std::domain_error(
        "Associativity does not evenly divide number of blocks. Encountered values were: associativity: " + argVec[4]
        + " Number of blocks: " + std::to_string(arguments.numBlocks));

  return arguments;
}

void Runner::printConfig() const {
  std::stringstream ss;
  ss << "====================================" << '\n';
  ss << "Cache protocol:\t\t" << args.protocol << '\n';
  ss << "Cache size:\t\t" << args.cacheSize << " B";

  if (args.cacheSize > (1 << 20))
    ss << " (" << std::setprecision(4) << static_cast<double>(args.cacheSize) / (1 << 20) << " MiB)";
  else if (args.cacheSize > (1 << 10))
    ss << " (" << std::setprecision(4) << static_cast<double>(args.cacheSize) / (1 << 10) << " KiB)";
  ss << '\n';

  ss << "Block size:\t\t" << args.blockSize << " B" << std::endl;

  ss << std::setw(20) << "Associativity:";

  if (args.associativity == 1) ss << "direct-mapped";
  else if (args.associativity == args.numBlocks) ss << "fully associative";
  else ss << std::to_string(args.associativity) << "-way set-associative";

  ss << "\n";

  ss << std::setw(20) << "Cache blocks:" << args.numBlocks << " (" << (args.numBlocks / args.associativity)
     << " per set)" << '\n';
  ss << "====================================";

  std::cout << ss.view() << std::endl;
}

void Runner::printStats() {
  auto coreMonitors = simSystem.coreMonitors();
  auto busMonitor = simSystem.busMonitor();

  std::stringstream header;

  header << std::left << std::setw(3) << pid_s << std::right << std::setw(exec_s.length() + 4) << exec_s
         << std::setw(comp_s.length() + 4) << comp_s << std::setw(idle_s.length() + 4) << idle_s
         << std::setw(load_s.length() + 6) << load_s << std::setw(stor_s.length() + 5) << stor_s
         << std::setw(hitc_s.length() + 4) << hitc_s << std::setw(misc_s.length() + 4) << misc_s
         << std::setw(misr_s.length() + 4) << misr_s << std::setw(prvt_s.length() + 4) << prvt_s
         << std::setw(shrd_s.length() + 4) << shrd_s << std::setw(ptrt_s.length() + 4) << ptrt_s;

  std::cout << "\x1b[91m" << header.view() << "\x1b[0m" << '\n';

  auto pid = 0u;
  std::stringstream coreStats;

  for (const auto& monitor : coreMonitors) {
    coreStats << std::left << std::setw(3) << pid << std::right << std::setw(exec_s.length() + 4) << monitor.cycleCount
              << std::setw(comp_s.length() + 4) << monitor.computeCycleCount << std::setw(idle_s.length() + 4)
              << monitor.idleCycleCount << std::setw(load_s.length() + 6) << monitor.loadCount
              << std::setw(stor_s.length() + 5) << monitor.storeCount << std::setw(hitc_s.length() + 4)
              << monitor.hitCount << std::setw(misc_s.length() + 4) << monitor.missCount
              << std::setw(misr_s.length() + 4) << std::setprecision(5)
              << static_cast<double>(monitor.missCount * 100) / (monitor.missCount + monitor.hitCount)
              << std::setw(prvt_s.length() + 4) << busMonitor.privateAccessCount[pid] << std::setw(shrd_s.length() + 4)
              << busMonitor.sharedAccessCount[pid] << std::setw(ptrt_s.length() + 4) << std::setprecision(5)
              << static_cast<double>(busMonitor.privateAccessCount[pid] * 100)
            / (busMonitor.privateAccessCount[pid] + busMonitor.sharedAccessCount[pid++])
              << '\n';
  }

  std::cout << coreStats.view() << std::endl;

  auto max = std::ranges::max(coreMonitors, {}, &CoreMonitor::cycleCount);
  std::cout << std::left << std::setw(20) << "Overall cycles:" << std::right << std::setw(12) << max.cycleCount << '\n';
  std::cout << std::left << std::setw(20) << "Bus traffic (B):" << std::right << std::setw(12) << busMonitor.dataTraffic
            << '\n';
  std::cout << std::left << std::setw(20) << "Invalidations:" << std::right << std::setw(12)
            << busMonitor.invalidationsCount << '\n';
  std::cout << std::left << std::setw(20) << "Updates:" << std::right << std::setw(12) << busMonitor.updatesCount
            << '\n';
  std::cout << std::left << std::setw(20) << "Writes back:" << std::right << std::setw(12) << busMonitor.writesbackCount
            << std::endl;
}

void Runner::writeToFile() {
  auto coreMonitors = simSystem.coreMonitors();
  auto busMonitor = simSystem.busMonitor();

  std::filesystem::path outputFilePath("output");

  outputFilePath /= args.benchmark;
  outputFilePath += '_';
  outputFilePath += args.protocol;
  outputFilePath += '_';
  outputFilePath += std::to_string(args.cacheSize);
  outputFilePath += '_';
  outputFilePath += std::to_string(args.associativity);
  outputFilePath += '_';
  outputFilePath += std::to_string(args.blockSize);
  outputFilePath.replace_extension("csv");

  std::filesystem::create_directories(outputFilePath);

  std::ofstream outputFile{outputFilePath};

  outputFile << pid_s << ',' << exec_s << ',' << comp_s << ',' << idle_s << ',' << load_s << ',' << stor_s << ','
             << hitc_s << ',' << misc_s << ',' << misr_s << ',' << prvt_s << ',' << shrd_s << ',' << ptrt_s << '\n';

  auto pid = 0u;
  for (const auto& monitor : coreMonitors) {
    outputFile << pid << ',' << monitor.cycleCount << ',' << monitor.computeCycleCount << ',' << monitor.idleCycleCount
               << ',' << monitor.loadCount << ',' << monitor.storeCount << ',' << monitor.hitCount << ','
               << monitor.missCount << ',' << std::setprecision(7)
               << static_cast<double>(monitor.missCount * 100) / (monitor.missCount + monitor.hitCount) << ','
               << busMonitor.privateAccessCount[pid] << ',' << busMonitor.sharedAccessCount[pid]
               << std::setw(ptrt_s.length() + 4) << std::setprecision(7)
               << static_cast<double>(busMonitor.privateAccessCount[pid] * 100)
            / (busMonitor.privateAccessCount[pid] + busMonitor.sharedAccessCount[pid++])
               << '\n';
  }
  outputFile.close();
}

auto Runner::start() { simSystem.run(); }

}// namespace CacheSim

int main(int argc, const char* argv[]) {
  try {
    CacheSim::Runner runner{CacheSim::Runner::checkArguments(argc, argv)};
    runner.printConfig();
    runner.start();
    runner.printStats();
    runner.writeToFile();
    return 0;
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
}
