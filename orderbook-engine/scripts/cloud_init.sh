#!/bin/bash
sudo apt update -y
sudo apt install -y build-essential cmake git linux-tools-common linux-tools-aws perf

# Clone repo
git clone https://github.com/yourusername/orderbook-engine.git /home/ubuntu/orderbook-engine

# Build orderbook engine
cd /home/ubuntu/orderbook-engine
mkdir -p build && cd build
cmake ..
make -j$(nproc)

# Run benchmark
./latency_benchmark > /home/ubuntu/benchmark_results.txt

# Shutdown instance
sudo shutdown -h now
