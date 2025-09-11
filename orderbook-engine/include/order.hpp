// order.hpp
#pragma once
#include <cstdint>

enum class OrderType { BUY, SELL };

struct Order {
    std::uint64_t id;
    OrderType type;
    double price;
    std::uint32_t quantity;
};


