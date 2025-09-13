#include <gtest/gtest.h>
#include "../include/orderbook.hpp"

class OrderBookTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Reset orderbook for each test
        ob = OrderBook();
    }
    void TearDown() override {}
    
    OrderBook ob;
};

TEST_F(OrderBookTest, AddBuyOrder) {
    Order buy_order{1, OrderType::BUY, 100.0, 10};
    
    // Should not throw
    EXPECT_NO_THROW(ob.add_order(buy_order));
}

TEST_F(OrderBookTest, AddSellOrder) {
    Order sell_order{1, OrderType::SELL, 200.0, 20};
    
    // Should not throw
    EXPECT_NO_THROW(ob.add_order(sell_order));
}

TEST_F(OrderBookTest, AddMultipleOrders) {
    Order order1{1, OrderType::BUY, 100.0, 10};
    Order order2{2, OrderType::SELL, 200.0, 20};
    Order order3{3, OrderType::BUY, 150.0, 15};
    
    EXPECT_NO_THROW(ob.add_order(order1));
    EXPECT_NO_THROW(ob.add_order(order2));
    EXPECT_NO_THROW(ob.add_order(order3));
}

TEST_F(OrderBookTest, MatchOrders) {
    Order buy_order{1, OrderType::BUY, 100.0, 10};
    Order sell_order{2, OrderType::SELL, 200.0, 20};
    
    ob.add_order(buy_order);
    ob.add_order(sell_order);
    
    // Should not throw
    EXPECT_NO_THROW(ob.match_orders());
}

TEST_F(OrderBookTest, MatchOrdersEmptyBook) {
    // Should not throw even with empty orderbook
    EXPECT_NO_THROW(ob.match_orders());
}

TEST_F(OrderBookTest, StressTest) {
    const int num_orders = 1000;
    
    for (int i = 0; i < num_orders; ++i) {
        OrderType type = (i % 2 == 0) ? OrderType::BUY : OrderType::SELL;
        Order order{static_cast<uint64_t>(i), type, 100.0 + i, static_cast<uint32_t>(i + 1)};
        
        EXPECT_NO_THROW(ob.add_order(order));
    }
    
    EXPECT_NO_THROW(ob.match_orders());
}
