#include <gtest/gtest.h>
#include "../include/memory_pool.hpp"
#include "../include/order.hpp"

class MemoryPoolTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(MemoryPoolTest, ThreadLocalPoolAllocation) {
    ThreadLocalPool<Order> pool(10);
    
    Order* order1 = pool.allocate();
    EXPECT_NE(order1, nullptr);
    
    Order* order2 = pool.allocate();
    EXPECT_NE(order2, nullptr);
    EXPECT_NE(order1, order2); // Should be different addresses
}

TEST_F(MemoryPoolTest, ThreadLocalPoolMultipleAllocations) {
    ThreadLocalPool<Order> pool(5);
    std::vector<Order*> orders;
    
    // Allocate multiple orders
    for (int i = 0; i < 10; ++i) {
        Order* order = pool.allocate();
        EXPECT_NE(order, nullptr);
        orders.push_back(order);
    }
    
    // All addresses should be different
    for (size_t i = 0; i < orders.size(); ++i) {
        for (size_t j = i + 1; j < orders.size(); ++j) {
            EXPECT_NE(orders[i], orders[j]);
        }
    }
}

TEST_F(MemoryPoolTest, ThreadLocalPoolInitialization) {
    ThreadLocalPool<Order> pool(100);
    
    // Should be able to allocate immediately
    Order* order = pool.allocate();
    EXPECT_NE(order, nullptr);
}

TEST_F(MemoryPoolTest, ThreadLocalPoolWithDifferentTypes) {
    ThreadLocalPool<int> int_pool(10);
    ThreadLocalPool<double> double_pool(10);
    
    int* int_ptr = int_pool.allocate();
    double* double_ptr = double_pool.allocate();
    
    EXPECT_NE(int_ptr, nullptr);
    EXPECT_NE(double_ptr, nullptr);
    
    // Should be able to use the allocated memory
    *int_ptr = 42;
    *double_ptr = 3.14;
    
    EXPECT_EQ(*int_ptr, 42);
    EXPECT_DOUBLE_EQ(*double_ptr, 3.14);
}

TEST_F(MemoryPoolTest, ThreadLocalPoolStressTest) {
    ThreadLocalPool<Order> pool(1000);
    std::vector<Order*> orders;
    
    // Allocate many orders
    for (int i = 0; i < 2000; ++i) {
        Order* order = pool.allocate();
        EXPECT_NE(order, nullptr);
        
        // Initialize the order
        order->id = i;
        order->type = (i % 2 == 0) ? OrderType::BUY : OrderType::SELL;
        order->price = 100.0 + i;
        order->quantity = i + 1;
        
        orders.push_back(order);
    }
    
    // Verify all orders were initialized correctly
    for (size_t i = 0; i < orders.size(); ++i) {
        EXPECT_EQ(orders[i]->id, i);
        EXPECT_EQ(orders[i]->type, (i % 2 == 0) ? OrderType::BUY : OrderType::SELL);
        EXPECT_DOUBLE_EQ(orders[i]->price, 100.0 + i);
        EXPECT_EQ(orders[i]->quantity, i + 1);
    }
}
