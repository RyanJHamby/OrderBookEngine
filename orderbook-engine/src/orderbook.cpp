// orderbook.cpp
#include "orderbook.hpp"

void OrderBook::add_order(const Order& order) {
    if (order.type == OrderType::BUY) {
        buy_orders.push_back(order);
    } else {
        sell_orders.push_back(order);
    }
}

void OrderBook::match_orders() {
    simd_price_match();
}

void OrderBook::simd_price_match() {
    // Skeleton: no-op for now; placeholder for SIMD matching
    (void)buy_orders;
    (void)sell_orders;
}


