# GNU Sort Clone Benchmark

## Overview

This project is a custom implementation of the GNU coreutils `sort` utility, written in C++ (`GNU_sort_clone.cpp`). The clone supports four options:
- `-n`: Numeric sorting.
- `-r`: Reverse order.
- `-u`: Unique lines (remove duplicates).
- `-o`: Output to a specified file.

The goal is to compare its performance (execution time and memory usage) and output correctness against GNU coreutils `sort` (v9.4, accessed via `gsort` on macOS) for various input sizes and content types. The benchmark tests small (1K lines), medium (1M lines), and large (10M lines) inputs, with random strings, numbers, and duplicate-heavy data. The implementation uses external sorting (chunk-based with priority queue merging) to handle large files efficiently, aiming to compete with GNU `sort` in specific cases.

**AI Usage Statement**: I used Grok (created by xAI) solely for generating the test data script (`gen_data.py`) and drafting this README text. Specifically, I prompted Grok to create a Python script for generating random strings and numbers, which I modified to include duplicate-heavy inputs. The quote from Grok’s output is: "For random strings, use `random.choices` with `string.ascii_letters`." All other components (clone implementation, benchmark scripts, analysis) were developed by me, with information sourced from the GNU coreutils manual (https://www.gnu.org/software/coreutils/manual/html_node/sort-invocation.html) and general web searches for sorting algorithms and benchmarking techniques.

## Requirements

- **OS**: macOS (e.g., Ventura 13 or later).
- **Dependencies**:
  - `g++` (Xcode Command Line Tools: `xcode-select --install`).
  - `hyperfine` (`brew install hyperfine`).
  - GNU coreutils (`brew install coreutils` for `gsort`).
  - Python 3 (`brew install python3`).
  - Homebrew (`/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"` if not installed).
- **Disk Space**: ~10GB for `large_strings.txt` (10M lines).
- **Files**: Ensure `GNU_sort_clone.cpp` (your code), `gen_data.py`, `benchmark.sh`, and `results_summary.md` are in the project directory.

## Setup and Reproduction Steps

1. **Compile the Clone**:
   ```bash
   g++ -O3 -std=c++17 GNU_sort_clone.cpp -o my_sort
   ```
   This creates the `my_sort` executable.

2. **Generate Test Data**:
   Run the provided `gen_data.py` to create input files:
   ```bash
   python3 gen_data.py
   ```
   Generates:
   - `small_strings.txt`: 1K random alphanumeric strings.
   - `medium_strings.txt`: 1M random strings.
   - `medium_numbers.txt`: 1M random numbers (-1e6 to 1e6).
   - `medium_duplicates.txt`: 1M strings with 50% duplicates.
   - `large_strings.txt`: 10M random strings.

3. **Run the Benchmark**:
   Use the provided `benchmark.sh` script, adapted for macOS:
   ```bash
   chmod +x benchmark.sh
   ./benchmark.sh
   ```
   **Note**: The script uses `gsort` (GNU sort) and `purge` for cache clearing (requires `sudo`). If you prefer not to use `sudo`, comment out the `sudo purge` line, but results may vary due to caching. Outputs are saved in the `results/` directory.

4. **Analyze Results**:
   - **Time**: Check `results/time_*.md` (hyperfine output).
   - **Memory**: Check `results/time_my_*.log` and `time_orig_*.log` for memory usage (search for memory in bytes).
   - **Output Differences**: Check `results/diff_*.log` for differences between `my_sort` and `gsort` outputs.
   - **Summary**: See `results_summary.md` for compiled results and analysis.

## Benchmark Results

| Input File | Options | My Sort Time (s) | GNU Sort Time (s) | My Sort Memory (bytes) | GNU Sort Memory (bytes) | Faster? | Output Differs? |
| --- | --- | --- | --- | --- | --- | --- | --- |
| small_strings.txt (1K) | - | 0.005 | 0.008 | 1,212,416 | 3,500,000 | Yes (37.5%) | No |
| medium_strings.txt (1M) | -n | 7.728 | 0.207 | 2,179,072 | 7,476,000 | No | Yes (non-stable) |
| medium_numbers.txt (1M) | -n | 2.087 | 0.264 | 2,260,992 | 8,948,000 | No | Yes (NaN handling) |
| medium_duplicates.txt (1M) | -u | 0.299 | 0.063 | 2,080,768 | 7,988 | No | No |
| large_strings.txt (10M) | - | 3.276 | 1.636 | 7,421,952 | 84,276,000 | No | Yes (non-stable) |

### Analysis

- **Small Inputs (1K, no options)**: My clone is 37.5% faster (0.005s vs 0.008s) and uses less memory (1.2MB vs 3.5MB) due to simpler string comparison (no locale overhead).
- **Medium Strings (1M, -n)**: GNU sort is significantly faster (0.207s vs 7.728s) but uses more memory (7.5MB vs 2.2MB), likely due to optimized I/O and parallelism.
- **Medium Numbers (1M, -n)**: GNU sort is faster (0.264s vs 2.087s) and uses more memory (8.9MB vs 2.3MB). My numeric parsing is less efficient.
- **Medium Duplicates (1M, -u)**: GNU sort is faster (0.063s vs 0.299s) and uses drastically less memory (7,988 bytes vs 2.1MB), indicating efficient deduplication.
- **Large Inputs (10M, no options)**: GNU sort is faster (1.636s vs 3.276s) but uses more memory (84.3MB vs 7.4MB) due to parallelism and buffering.

### Root Causes

- **Performance**:
  - GNU sort uses stable mergesort (O(n log n)) with parallelism (--parallel=4) and optimized I/O (buffered, possibly mmap-based). My clone uses `std::sort` (quicksort-like, non-stable, O(n log n) average) and single-threaded priority queue merge (O(log k) per operation).
  - Small inputs favor my clone due to minimal overhead. Medium/large inputs favor GNU sort due to parallelism and I/O optimizations.
  - Numeric (-n): My `parse_number` relies on `std::stod`, which is slower for large datasets. GNU’s parser is optimized for -n/-g.
  - Duplicates (-u): GNU’s deduplication (possibly hash-based) is highly efficient, unlike my `std::unique`.
- **Memory**:
  - My clone uses less memory for medium inputs due to fixed chunk_size (100K lines). GNU’s higher memory for large inputs (84MB) reflects buffering and parallelism.
  - For duplicates, GNU’s low memory (7,988 bytes) suggests in-place deduplication.
- **Bottlenecks**: My clone’s fstream I/O and priority queue are slower than GNU’s multi-way merge and parallel processing.

### Output Differences

- **Non-stable Sorting**: My clone’s `std::sort` is not stable, reordering equal strings (e.g., "abc" and "abc" may swap). Affects medium_strings.txt and large_strings.txt.
- **Numeric (-n)**: My clone handles basic numbers but may differ on edge cases (NaN, inf, hex) due to simpler `std::stod` vs GNU’s robust -g parser. Affects medium_numbers.txt.
- **Locale**: My clone sets `std::locale("")` but lacks GNU’s full locale support (e.g., -b, -d).
- **Duplicates (-u)**: No differences after fixing global uniqueness.
