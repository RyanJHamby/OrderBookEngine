#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$ROOT_DIR/build"

cmake -S "$ROOT_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release -DORDERBOOK_BUILD_BENCHMARKS=ON -DORDERBOOK_ENABLE_LTO=ON | cat
cmake --build "$BUILD_DIR" --config Release -j "$(sysctl -n hw.ncpu 2>/dev/null || nproc || echo 4)" | cat

"$BUILD_DIR/latency_benchmark"


