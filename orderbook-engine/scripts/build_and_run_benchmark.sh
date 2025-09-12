#!/bin/bash
set -e

# Go to project root (this script is in scripts/, so cd up one level)
cd "$(dirname "$0")/.."

# Clean and recreate build directory
rm -rf build
mkdir build
cd build

# Configure with CMake
cmake ..

# Build everything with parallel jobs
make -j$(sysctl -n hw.ncpu)

# Run the main engine
echo "=== Running orderbook ==="
./orderbook || true

# Run the latency benchmark
echo "=== Running latency benchmark ==="
./latency_benchmark || true
