// orderbook.hpp
#pragma once

#include "order.hpp"
#include <vector>
#include <immintrin.h>

class OrderBook {
public:
    void add_order(const Order& order);
    void match_orders();

private:
    std::vector<Order> buy_orders;
    std::vector<Order> sell_orders;

    void simd_price_match();
};


