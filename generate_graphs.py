import pandas
import matplotlib.pyplot
import numpy
import os

# Possible options for cache configuration
protocols = ["MESI", "Dragon", "MOESI"]
# Possible benchmarks
benchmarks = ["bodytrack", "blackscholes", "fluidanimate"]
# Cache size; minimum: 16 B (4 words); maximum: 1 MiB
cache_sizes = [2 ** i for i in range(4, 21)]
# Associativity: direct-mapped to maximum possible
associativities = [2 ** i for i in range(0, 21)]
# Cache line/block size: minimum: 4 words (16 B); maximum: 512 words (2048 B = 2 KiB)
line_sizes = [2 ** i for i in range(4, 12)]

# Get list of files in output folder
files = os.listdir("output")
# Prepare for dataframes of pids
listofdicts = [[] for i in range(0, 4)]
dfs = []

# Read in all CSVs, add in configuration to each, split them by PID and insert into corresponding lists
for csv_file in files:
    config = os.path.splitext(csv_file)[0].split("_")
    read_csv = pandas.read_csv("output/" + csv_file).to_dict(orient="records")
    for row in read_csv:
        row |= {"benchmark": config[0], "protocol": config[1], "cache_size": int(config[2]),
                "associativity": int(config[3]), "line_size": int(config[4])}
        del row["pid"]
    listofdicts[0].append(read_csv[0])
    listofdicts[1].append(read_csv[1])
    listofdicts[2].append(read_csv[2])
    listofdicts[3].append(read_csv[3])

dfs.append(pandas.DataFrame.from_records(listofdicts[0]))
dfs.append(pandas.DataFrame.from_records(listofdicts[1]))
dfs.append(pandas.DataFrame.from_records(listofdicts[2]))
dfs.append(pandas.DataFrame.from_records(listofdicts[3]))

df3 = dfs[3]

# MISS RATE ON CACHE SIZE FOR PID 3
# # Keep line size = 32 B, associativity = 2, vary cache sizes
# for benchmark in benchmarks:
#     for protocol in protocols:
#         filtered = df3.loc[(df3["protocol"] == protocol) & (df3["benchmark"] == benchmark) & (
#                 df3["line_size"] == 32) & (df3["associativity"] == 2)]
#         filtered.to_csv(f"miss_rate_cache_size_{benchmark}_{protocol}.csv")

# MISS RATE ON ASSOCIATIVITY FOR PID 3
# Keep line size = 32 B, cache size = 32 KiB B, vary associativities
# for benchmark in benchmarks:
#     for protocol in protocols:
#         filtered = df3.loc[(df3["protocol"] == protocol) & (df3["benchmark"] == benchmark) & (
#                 df3["line_size"] == 32) & (df3["cache_size"] == 4096)]
#         filtered.to_csv(f"report/csvs/miss_rate_associativity_{benchmark}_{protocol}.csv")

# MISS RATE ON LINE SIZE FOR PID 3
# Keep cache size = 32 KiB B, associativity = 2, vary line sizes
for benchmark in benchmarks:
    for protocol in protocols:
        filtered = df3.loc[(df3["protocol"] == protocol) & (df3["benchmark"] == benchmark) & (
                df3["associativity"] == 2) & (df3["cache_size"] == 8192)]
        filtered.to_csv(f"report/csvs/miss_rate_line_size_{benchmark}_{protocol}.csv")
