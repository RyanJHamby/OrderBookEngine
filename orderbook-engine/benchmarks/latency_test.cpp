// latency_test.cpp
#include "orderbook.hpp"
#include <chrono>
#include <cstdint>
#include <iostream>

using clock_type = std::chrono::steady_clock;

int main() {
    constexpr std::size_t iterations = 1000000;
    ob::OrderBook book;

    auto start = clock_type::now();
    for (std::size_t i = 0; i < iterations; ++i) {
        Order o{static_cast<std::uint64_t>(i), (i & 1) ? OrderType::BUY : OrderType::SELL, 1000.0 + static_cast<double>(i % 100), static_cast<std::uint32_t>(1)};
        book.add(o);
        if ((i % 7) == 0) {
            (void)book.cancel(static_cast<std::uint64_t>(i / 2));
        }
    }
    auto end = clock_type::now();
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    double per_op_ns = static_cast<double>(ns) / static_cast<double>(iterations);

    std::cout << "Ran " << iterations << " ops in " << ns << " ns (" << per_op_ns << " ns/op)\n";
    return 0;
}


