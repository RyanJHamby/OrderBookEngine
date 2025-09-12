#!/bin/bash
set -e

# Update and install dependencies
sudo apt update -y
sudo apt install -y build-essential cmake git linux-tools-common linux-tools-aws perf awscli -y

# Set variables
PROJECT_DIR="/home/ubuntu/orderbook-engine"
mkdir -p "$PROJECT_DIR"
cd "$PROJECT_DIR"

# Clone repo if needed
git clone https://github.com/yourusername/orderbook-engine.git "$PROJECT_DIR" || echo "Repo already exists"

# Build project
mkdir -p build && cd build
cmake ..
make -j$(nproc)

# Run benchmark
./latency_benchmark > /home/ubuntu/benchmark_results.txt 2>&1

# Get AWS Account ID for bucket naming
ACCOUNT_ID=$(curl -s http://169.254.169.254/latest/dynamic/instance-identity/document | jq -r '.accountId')

# Define bucket name
BUCKET_NAME="orderbook-benchmark-$ACCOUNT_ID"

# Create bucket if it doesn't exist
aws s3api head-bucket --bucket "$BUCKET_NAME" 2>/dev/null || \
aws s3 mb "s3://$BUCKET_NAME"

# Upload results
aws s3 cp /home/ubuntu/benchmark_results.txt "s3://$BUCKET_NAME/benchmark_results_$(date +%s).txt"

# Optional: log upload
echo "Benchmark results uploaded to s3://$BUCKET_NAME/"

# Shutdown instance
sudo shutdown -h now
