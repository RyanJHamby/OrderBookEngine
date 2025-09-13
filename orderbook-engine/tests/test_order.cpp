#include <gtest/gtest.h>
#include "../include/order.hpp"

class OrderTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(OrderTest, OrderCreation) {
    Order order{1, OrderType::BUY, 100.5, 50};
    
    EXPECT_EQ(order.id, 1);
    EXPECT_EQ(order.type, OrderType::BUY);
    EXPECT_DOUBLE_EQ(order.price, 100.5);
    EXPECT_EQ(order.quantity, 50);
}

TEST_F(OrderTest, OrderTypes) {
    Order buy_order{1, OrderType::BUY, 100.0, 10};
    Order sell_order{2, OrderType::SELL, 200.0, 20};
    
    EXPECT_EQ(buy_order.type, OrderType::BUY);
    EXPECT_EQ(sell_order.type, OrderType::SELL);
    EXPECT_NE(buy_order.type, sell_order.type);
}

TEST_F(OrderTest, OrderComparison) {
    Order order1{1, OrderType::BUY, 100.0, 10};
    Order order2{1, OrderType::BUY, 100.0, 10};
    Order order3{2, OrderType::SELL, 200.0, 20};
    
    // Same order data should have same values
    EXPECT_EQ(order1.id, order2.id);
    EXPECT_EQ(order1.type, order2.type);
    EXPECT_DOUBLE_EQ(order1.price, order2.price);
    EXPECT_EQ(order1.quantity, order2.quantity);
    
    // Different orders should have different IDs
    EXPECT_NE(order1.id, order3.id);
}
