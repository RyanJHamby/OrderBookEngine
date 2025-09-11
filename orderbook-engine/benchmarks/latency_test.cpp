// latency_test.cpp
#include "orderbook.hpp"
#include <chrono>
#include <cstdint>
#include <iostream>

using clock_type = std::chrono::steady_clock;

int main() {
    constexpr std::size_t iterations = 1000000;
    OrderBook book;

    auto start = clock_type::now();
    for (std::size_t i = 0; i < iterations; ++i) {
        Order o{static_cast<std::uint64_t>(i), (i & 1) ? OrderType::BUY : OrderType::SELL, 1000.0 + static_cast<double>(i % 100), static_cast<std::uint32_t>(1)};
        book.add_order(o);
        if ((i % 7) == 0) {
            // no cancel in skeleton; simulate extra work
            book.match_orders();
        }
    }
    auto end = clock_type::now();
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    double per_op_ns = static_cast<double>(ns) / static_cast<double>(iterations);

    std::cout << "Ran " << iterations << " ops in " << ns << " ns (" << per_op_ns << " ns/op)\n";
    return 0;
}


