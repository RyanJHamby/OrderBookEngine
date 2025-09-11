// main.cpp
#include "orderbook.hpp"
#include <iostream>

int main() {
    OrderBook book;
    Order o{1, OrderType::BUY, 100.0, 10};
    book.add_order(o);
    book.match_orders();
    std::cout << "OK\n";
    return 0;
}


