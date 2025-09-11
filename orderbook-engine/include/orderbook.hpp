// orderbook.hpp
#pragma once

#include "order.hpp"
#include <cstdint>
#include <optional>

namespace ob {

class OrderBook {
public:
    OrderBook() = default;
    ~OrderBook() = default;

    bool add(const ::Order& order);
    bool cancel(std::uint64_t orderId);
    std::optional<::Order> bestBid() const;
    std::optional<::Order> bestAsk() const;
    std::size_t size() const noexcept;
};

} // namespace ob


