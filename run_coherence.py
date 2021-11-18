import sys
from functools import partial
from itertools import product, cycle
from multiprocessing import Pool, cpu_count, freeze_support
from subprocess import run

from pandas import DataFrame


# Is debug mode?
def debugger_is_active() -> bool:
    """Return if the debugger is currently active"""
    gettrace = getattr(sys, "gettrace", lambda: None)
    return gettrace() is not None


# Define a parallel wrapper with a partial function, to accelerate things
def parallel_wrapper(worker, count):
    def parallelise(worker2, work_items):
        with Pool(count) as pool:
            if __name__ == '__main__':
                freeze_support()
                pool.map(worker2, work_items)

    return partial(parallelise, worker)


# coherence runner worker
def coherence_worker(config: dict):
    run(args=["cmake-build-release/src/coherence",
              str(config["cache_type"]),
              str(config["benchmark"]),
              str(config["cache_size"]),
              str(config["associativity"]),
              str(config["line_size"])])
    return


# Output directory for cache writing
results_output_dir = ". /output/results"
# Possible options for cache configuration
cache_types = ["Dragon", "MOESI"]
# Possible benchmarks
benchmarks = ["bodytrack", "blackscholes", "fluidanimate"]
# Cache size; minimum: 16 B (4 words); maximum: 512 MB
cache_size = [2 ** i for i in range(4, 30)]
# Associativity: direct-mapped to maximum possible
associativity = [2 ** i for i in range(0, 30)]
# Cache line/block size: minimum: 4 words (16 B); maximum: 256 words (1024 B = 1 KiB)
line_size = [2 ** i for i in range(4, 11)]

# coherence CSV headers for corresponding resources
option_names = [
    "cache_type",
    "benchmark",
    "cache_size",
    "associativity",
    "line_size",
]

# Combined list of options
all_options = [
    cache_types,
    benchmarks,
    cache_size,
    associativity,
    line_size
]

# Cartesian product of option lists into dictionaries, with keys being the
# command-line option strings, and values being the values themselves
configs = [dict(zip(option_names, cycle(vals))) for vals in product(*all_options)]
# Filter configurations so only valid ones pass through
configs = [setup for setup in configs if
           (setup["cache_size"] >= setup["line_size"]) and (
                   setup["cache_size"] / setup["line_size"] % setup["associativity"] == 0)]
config_permutations = DataFrame(configs)

# Run the parallel workers
if debugger_is_active():
    # Debug mode: test on one file
    coherence_worker(config_permutations.iloc[0].to_dict())
else:
    # Run mode: run in parallel
    parallel_wrapper(coherence_worker, cpu_count())(
        config_permutations.to_dict("records")
    )
