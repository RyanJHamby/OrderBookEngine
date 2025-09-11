// orderbook.hpp
#pragma once

#include "order.hpp"
#include <vector>
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386) || defined(_M_IX86)
#include <immintrin.h>
#endif

class OrderBook {
public:
    void add_order(const Order& order);
    void match_orders();

private:
    std::vector<Order> buy_orders;
    std::vector<Order> sell_orders;

    void simd_price_match();
};


