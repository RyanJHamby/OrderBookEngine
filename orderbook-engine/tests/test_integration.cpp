#include <gtest/gtest.h>
#include "../include/orderbook.hpp"
#include "../include/order_queue.hpp"
#include "../include/memory_pool.hpp"
#include <thread>
#include <vector>
#include <random>

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        ob = OrderBook();
        pool = std::make_unique<ThreadLocalPool<Order>>(1000);
    }
    void TearDown() override {}
    
    OrderBook ob;
    std::unique_ptr<ThreadLocalPool<Order>> pool;
};

TEST_F(IntegrationTest, OrderBookWithMemoryPool) {
    // Allocate orders from memory pool
    Order* buy_order = pool->allocate();
    Order* sell_order = pool->allocate();
    
    buy_order->id = 1;
    buy_order->type = OrderType::BUY;
    buy_order->price = 100.0;
    buy_order->quantity = 10;
    
    sell_order->id = 2;
    sell_order->type = OrderType::SELL;
    sell_order->price = 200.0;
    sell_order->quantity = 20;
    
    // Add to orderbook
    EXPECT_NO_THROW(ob.add_order(*buy_order));
    EXPECT_NO_THROW(ob.add_order(*sell_order));
    
    // Match orders
    EXPECT_NO_THROW(ob.match_orders());
}

TEST_F(IntegrationTest, OrderBookWithQueue) {
    LockFreeQueue<Order, 100> queue;
    
    // Generate orders
    std::vector<Order> orders;
    for (int i = 0; i < 50; ++i) {
        Order order{static_cast<uint64_t>(i), 
                   (i % 2 == 0) ? OrderType::BUY : OrderType::SELL,
                   100.0 + i, 
                   static_cast<uint32_t>(i + 1)};
        orders.push_back(order);
    }
    
    // Push orders to queue
    for (const auto& order : orders) {
        EXPECT_TRUE(queue.push(order));
    }
    
    // Pop orders and add to orderbook
    Order popped_order{0, OrderType::BUY, 0.0, 0};
    int count = 0;
    while (queue.pop(popped_order) && count < 50) {
        EXPECT_NO_THROW(ob.add_order(popped_order));
        count++;
    }
    
    EXPECT_EQ(count, 50);
    EXPECT_NO_THROW(ob.match_orders());
}

TEST_F(IntegrationTest, FullPipeline) {
    LockFreeQueue<Order, 1000> queue;
    ThreadLocalPool<Order> pool(1000);
    
    const int num_orders = 100;
    
    // Generate orders using memory pool
    std::vector<Order*> allocated_orders;
    for (int i = 0; i < num_orders; ++i) {
        Order* order = pool.allocate();
        order->id = i;
        order->type = (i % 2 == 0) ? OrderType::BUY : OrderType::SELL;
        order->price = 100.0 + (i % 50);
        order->quantity = (i % 20) + 1;
        allocated_orders.push_back(order);
    }
    
    // Push to queue
    for (Order* order : allocated_orders) {
        EXPECT_TRUE(queue.push(*order));
    }
    
    // Process through orderbook
    Order popped_order{0, OrderType::BUY, 0.0, 0};
    int processed = 0;
    while (queue.pop(popped_order)) {
        EXPECT_NO_THROW(ob.add_order(popped_order));
        processed++;
    }
    
    EXPECT_EQ(processed, num_orders);
    EXPECT_NO_THROW(ob.match_orders());
}

TEST_F(IntegrationTest, ConcurrentOrderGeneration) {
    LockFreeQueue<Order, 1000> queue;
    const int orders_per_thread = 25;
    const int num_threads = 4;
    
    std::vector<std::thread> threads;
    
    // Generate orders concurrently
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([&queue, t, orders_per_thread]() {
            for (int i = 0; i < orders_per_thread; ++i) {
                Order order{static_cast<uint64_t>(t * orders_per_thread + i),
                           (i % 2 == 0) ? OrderType::BUY : OrderType::SELL,
                           100.0 + i, 
                           static_cast<uint32_t>(i + 1)};
                queue.push(order);
            }
        });
    }
    
    // Wait for all threads
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Process all orders
    Order popped_order{0, OrderType::BUY, 0.0, 0};
    int total_processed = 0;
    while (queue.pop(popped_order)) {
        EXPECT_NO_THROW(ob.add_order(popped_order));
        total_processed++;
    }
    
    EXPECT_EQ(total_processed, num_threads * orders_per_thread);
    EXPECT_NO_THROW(ob.match_orders());
}
