#!/bin/bash
set -e

echo "Running OrderBook Engine Tests"
echo "=============================="

# Clean and create build directory
rm -rf build
mkdir -p build
cd build

# Configure with CMake
echo "Configuring project..."
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build the project
echo "Building project..."
make -j$(nproc)

# Run tests
echo "Running tests..."
./tests/orderbook_tests

echo "All tests completed successfully!"
