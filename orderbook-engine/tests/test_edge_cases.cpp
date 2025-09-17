#include <gtest/gtest.h>
#include "../include/orderbook.hpp"
#include "../include/order_queue.hpp"
#include "../include/memory_pool.hpp"
#include <limits>

class EdgeCaseTest : public ::testing::Test {
protected:
    void SetUp() override {
        ob = OrderBook();
    }
    void TearDown() override {}
    
    OrderBook ob;
};

TEST_F(EdgeCaseTest, OrderExtremeValues) {
    // Test with maximum values
    Order max_order{
        std::numeric_limits<uint64_t>::max(),
        OrderType::BUY,
        std::numeric_limits<double>::max(),
        std::numeric_limits<uint32_t>::max()
    };
    
    EXPECT_NO_THROW(ob.add_order(max_order));
    EXPECT_NO_THROW(ob.match_orders());
}

TEST_F(EdgeCaseTest, OrderZeroValues) {
    // Test with zero values
    Order zero_order{0, OrderType::SELL, 0.0, 0};
    
    EXPECT_NO_THROW(ob.add_order(zero_order));
    EXPECT_NO_THROW(ob.match_orders());
}

TEST_F(EdgeCaseTest, OrderNegativePrice) {
    // Test with negative price (edge case)
    Order negative_price_order{1, OrderType::BUY, -100.0, 10};
    
    EXPECT_NO_THROW(ob.add_order(negative_price_order));
    EXPECT_NO_THROW(ob.match_orders());
}

TEST_F(EdgeCaseTest, OrderVerySmallPrice) {
    // Test with very small price
    Order small_price_order{1, OrderType::SELL, 0.000001, 1};
    
    EXPECT_NO_THROW(ob.add_order(small_price_order));
    EXPECT_NO_THROW(ob.match_orders());
}

TEST_F(EdgeCaseTest, OrderBookEmptyOperations) {
    // Test operations on empty orderbook
    EXPECT_NO_THROW(ob.match_orders());
    
    // Add single order
    Order single_order{1, OrderType::BUY, 100.0, 10};
    EXPECT_NO_THROW(ob.add_order(single_order));
    EXPECT_NO_THROW(ob.match_orders());
}

TEST_F(EdgeCaseTest, OrderBookSingleOrderType) {
    // Test with only buy orders
    for (int i = 0; i < 10; ++i) {
        Order buy_order{static_cast<uint64_t>(i), OrderType::BUY, 100.0 + i, 10};
        EXPECT_NO_THROW(ob.add_order(buy_order));
    }
    EXPECT_NO_THROW(ob.match_orders());
    
    // Test with only sell orders
    OrderBook ob2;
    for (int i = 0; i < 10; ++i) {
        Order sell_order{static_cast<uint64_t>(i), OrderType::SELL, 200.0 + i, 10};
        EXPECT_NO_THROW(ob2.add_order(sell_order));
    }
    EXPECT_NO_THROW(ob2.match_orders());
}

TEST_F(EdgeCaseTest, OrderBookIdenticalOrders) {
    // Test with identical orders
    Order identical_order{1, OrderType::BUY, 100.0, 10};
    
    for (int i = 0; i < 5; ++i) {
        EXPECT_NO_THROW(ob.add_order(identical_order));
    }
    EXPECT_NO_THROW(ob.match_orders());
}

TEST_F(EdgeCaseTest, LockFreeQueueEdgeCases) {
    LockFreeQueue<Order, 1> tiny_queue;
    
    Order order1{1, OrderType::BUY, 100.0, 10};
    Order order2{2, OrderType::SELL, 200.0, 20};
    
    // Should be able to push one order
    EXPECT_TRUE(tiny_queue.push(order1));
    
    // Should fail to push second order (queue full)
    EXPECT_FALSE(tiny_queue.push(order2));
    
    // Should be able to pop the first order
    Order popped{0, OrderType::SELL, 0.0, 0};
    EXPECT_TRUE(tiny_queue.pop(popped));
    EXPECT_EQ(popped.id, 1);
    
    // Should fail to pop from empty queue
    EXPECT_FALSE(tiny_queue.pop(popped));
}

TEST_F(EdgeCaseTest, LockFreeQueueCircularWrap) {
    LockFreeQueue<Order, 3> small_queue;
    
    // Fill queue completely
    Order order1{1, OrderType::BUY, 100.0, 10};
    Order order2{2, OrderType::SELL, 200.0, 20};
    Order order3{3, OrderType::BUY, 300.0, 30};
    
    EXPECT_TRUE(small_queue.push(order1));
    EXPECT_TRUE(small_queue.push(order2));
    EXPECT_TRUE(small_queue.push(order3));
    
    // Try to push one more (should fail)
    Order order4{4, OrderType::SELL, 400.0, 40};
    EXPECT_FALSE(small_queue.push(order4));
    
    // Pop one to make room
    Order popped{0, OrderType::SELL, 0.0, 0};
    EXPECT_TRUE(small_queue.pop(popped));
    EXPECT_EQ(popped.id, 1);
    
    // Should now be able to push
    EXPECT_TRUE(small_queue.push(order4));
}

TEST_F(EdgeCaseTest, MemoryPoolEdgeCases) {
    ThreadLocalPool<Order> pool(0); // Zero size pool
    
    // Should still be able to allocate (grows dynamically)
    Order* order = pool.allocate();
    EXPECT_NE(order, nullptr);
    
    // Test with very large pool
    ThreadLocalPool<Order> large_pool(1000000);
    
    std::vector<Order*> allocations;
    for (int i = 0; i < 1000; ++i) {
        Order* order = large_pool.allocate();
        EXPECT_NE(order, nullptr);
        allocations.push_back(order);
    }
    
    // All allocations should be unique
    for (size_t i = 0; i < allocations.size(); ++i) {
        for (size_t j = i + 1; j < allocations.size(); ++j) {
            EXPECT_NE(allocations[i], allocations[j]);
        }
    }
}

TEST_F(EdgeCaseTest, OrderBookStressTest) {
    const int num_orders = 100000;
    
    // Add many orders rapidly
    for (int i = 0; i < num_orders; ++i) {
        Order order{static_cast<uint64_t>(i),
                   (i % 2 == 0) ? OrderType::BUY : OrderType::SELL,
                   100.0 + (i % 1000),
                   static_cast<uint32_t>((i % 100) + 1)};
        EXPECT_NO_THROW(ob.add_order(order));
    }
    
    // Match all orders
    EXPECT_NO_THROW(ob.match_orders());
}

TEST_F(EdgeCaseTest, OrderBookAlternatingPatterns) {
    // Test alternating buy/sell pattern
    for (int i = 0; i < 100; ++i) {
        OrderType type = (i % 2 == 0) ? OrderType::BUY : OrderType::SELL;
        Order order{static_cast<uint64_t>(i), type, 100.0 + i, 10};
        EXPECT_NO_THROW(ob.add_order(order));
    }
    EXPECT_NO_THROW(ob.match_orders());
    
    // Test all buys then all sells
    OrderBook ob2;
    for (int i = 0; i < 50; ++i) {
        Order buy_order{static_cast<uint64_t>(i), OrderType::BUY, 100.0 + i, 10};
        EXPECT_NO_THROW(ob2.add_order(buy_order));
    }
    for (int i = 50; i < 100; ++i) {
        Order sell_order{static_cast<uint64_t>(i), OrderType::SELL, 200.0 + i, 10};
        EXPECT_NO_THROW(ob2.add_order(sell_order));
    }
    EXPECT_NO_THROW(ob2.match_orders());
}
