#!/bin/bash

# Prerequisites: Install hyperfine (`brew install hyperfine`), coreutils (`brew install coreutils`), ensure `my_sort` and `gsort` are in PATH
# Clear system cache (macOS equivalent)
clear_cache() {
    sync
    sudo purge > /dev/null 2>&1  # Requires admin privileges; skip if not needed
}

# Test cases
inputs=("small_strings.txt" "medium_strings.txt" "medium_numbers.txt" "medium_duplicates.txt" "large_strings.txt")
options=("" "-n" "-r" "-u" "-nru")

# Create output directory
mkdir -p results

for input in "${inputs[@]}"; do
    for opt in "${options[@]}"; do
        clear_cache
        echo "Testing $input with options: $opt"
        hyperfine --warmup 1 --runs 5 "./my_sort $opt $input > results/my_out_${input}_${opt}.txt" \
                  "gsort $opt $input > results/orig_out_${input}_${opt}.txt" \
                  --export-markdown results/time_${input}_${opt}.md
        diff results/my_out_${input}_${opt}.txt results/orig_out_${input}_${opt}.txt > results/diff_${input}_${opt}.log
        /usr/bin/time -l ./my_sort $opt $input > /dev/null 2> results/time_my_${input}_${opt}.log
        /usr/bin/time -l gsort $opt $input > /dev/null 2> results/time_orig_${input}_${opt}.log
    done
done

# For large file, test GNU sort with parallel
clear_cache
hyperfine --warmup 1 --runs 5 "./my_sort large_strings.txt > results/my_out_large.txt" \
          "gsort --parallel=4 large_strings.txt > results/orig_out_large.txt" \
          --export-markdown results/time_large_parallel.md