#!/bin/bash
set -e

cd "$(dirname "$0")/.."

rm -rf build
mkdir build
cd build

cmake ..
make -j$(sysctl -n hw.ncpu)

echo "=== Running orderbook ==="
./orderbook || true

echo "=== Running latency benchmark ==="
./latency_benchmark || true
