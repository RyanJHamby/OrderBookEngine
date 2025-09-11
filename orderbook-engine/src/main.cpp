// main.cpp
#include "orderbook.hpp"
#include <iostream>

int main() {
    ob::OrderBook obook;
    std::cout << "OrderBook size: " << obook.size() << "\n";
    return 0;
}


