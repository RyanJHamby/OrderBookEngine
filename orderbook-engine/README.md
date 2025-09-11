# orderbook-engine

Minimal skeleton for a low-latency C++ order book with benchmarks.

## Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DORDERBOOK_BUILD_BENCHMARKS=ON
cmake --build build -j
```

Run the main binary:
```bash
./build/orderbook-engine
```

## Benchmark

```bash
./scripts/run_benchmark.sh
```

## Layout

- include/: headers
- src/: sources and `main.cpp`
- benchmarks/: `latency_test.cpp`
- scripts/: helper scripts
- CMakeLists.txt: build config

