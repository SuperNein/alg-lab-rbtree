#!/usr/bin/env bash
set -euo pipefail

VER="${1:-v1}"
DATA="benchmarks/data"
OUT="report/raw/${VER}"
mkdir -p "$OUT"

# Build
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release > /dev/null
cmake --build build-release -j

cmake -S . -B build-prof -DCMAKE_BUILD_TYPE=Release -DENABLE_GPROF=ON > /dev/null
cmake --build build-prof -j

# Generating data
[ -f "$DATA/01_random.in" ] || python3 benchmarks/gen_bench.py --out "$DATA" --ops 500000

# Wall-Clock & Peak RSS
: > "$OUT/time_rss.txt"
for f in "$DATA"/*.in; do
    echo "=== $(basename "$f" .in) ===" >> "$OUT/time_rss.txt"
    /usr/bin/time -v ./build-release/dict < "$f" > /dev/null 2>> "$OUT/time_rss.txt"
done

# gprof CPU profile
for f in "$DATA"/*.in; do
    ./build-prof/dict < "$f" > /dev/null
    gprof ./build-prof/dict gmon.out > "$OUT/gprof_$(basename "$f" .in).txt"
done
rm -f gmon.out

# valgrind memcheck
head -n 20000 "$DATA/01_random.in" | valgrind --leak-check=full \
    ./build-release/dict > /dev/null 2> "$OUT/memcheck.txt"

# massif
valgrind --tool=massif --massif-out-file="$OUT/massif.out" \
    ./build-release/dict < "$DATA/01_random.in" > /dev/null 2>&1
ms_print "$OUT/massif.out" > "$OUT/massif.txt"

rm -f /tmp/rb_bench_*.bin
