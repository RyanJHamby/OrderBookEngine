// latency_test.cpp
#include "orderbook.hpp"
#include <chrono>
#include <iostream>
#include <random>
#include <vector>

int main() {
    OrderBook ob;
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> price_dist(100, 200);
    std::uniform_int_distribution<int> qty_dist(1, 100);

    const int NUM_ORDERS = 1'000'000;
    std::vector<Order> orders;
    orders.reserve(NUM_ORDERS);

    for (int i = 0; i < NUM_ORDERS; ++i) {
        orders.push_back(Order{static_cast<std::uint64_t>(i), i % 2 ? OrderType::BUY : OrderType::SELL, price_dist(rng), static_cast<std::uint32_t>(qty_dist(rng))});
    }

    auto start = std::chrono::high_resolution_clock::now();
    for (auto& order : orders) ob.add_order(order);
    ob.match_orders();
    auto end = std::chrono::high_resolution_clock::now();

    double latency_us = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1'000.0 / NUM_ORDERS;
    std::cout << "Average latency per match: " << latency_us << " us\n";
}


