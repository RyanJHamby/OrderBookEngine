// orderbook.cpp
#include "orderbook.hpp"

namespace ob {

bool OrderBook::add(const ::Order& /*order*/) { return true; }
bool OrderBook::cancel(std::uint64_t /*orderId*/) { return true; }
std::optional<::Order> OrderBook::bestBid() const { return std::nullopt; }
std::optional<::Order> OrderBook::bestAsk() const { return std::nullopt; }
std::size_t OrderBook::size() const noexcept { return 0U; }

} // namespace ob


