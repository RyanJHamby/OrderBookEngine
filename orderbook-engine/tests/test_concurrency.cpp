#include <gtest/gtest.h>
#include "../include/orderbook.hpp"
#include "../include/order_queue.hpp"
#include "../include/memory_pool.hpp"
#include <thread>
#include <vector>
#include <atomic>
#include <random>

class ConcurrencyTest : public ::testing::Test {
protected:
    void SetUp() override {
        ob = OrderBook();
        pool = std::make_unique<ThreadLocalPool<Order>>(10000);
    }
    void TearDown() override {}
    
    OrderBook ob;
    std::unique_ptr<ThreadLocalPool<Order>> pool;
    std::atomic<int> order_counter{0};
};

TEST_F(ConcurrencyTest, ConcurrentOrderAddition) {
    const int num_threads = 4;
    const int orders_per_thread = 1000;
    std::vector<std::thread> threads;
    
    // Launch threads that add orders concurrently
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([this, t, orders_per_thread]() {
            for (int i = 0; i < orders_per_thread; ++i) {
                Order order{static_cast<uint64_t>(t * orders_per_thread + i),
                           (i % 2 == 0) ? OrderType::BUY : OrderType::SELL,
                           100.0 + i,
                           static_cast<uint32_t>(i + 1)};
                ob.add_order(order);
                order_counter.fetch_add(1);
            }
        });
    }
    
    // Wait for all threads
    for (auto& thread : threads) {
        thread.join();
    }
    
    EXPECT_EQ(order_counter.load(), num_threads * orders_per_thread);
    EXPECT_NO_THROW(ob.match_orders());
}

TEST_F(ConcurrencyTest, ConcurrentQueueOperations) {
    LockFreeQueue<Order, 10000> queue;
    const int num_threads = 4;
    const int operations_per_thread = 500;
    std::atomic<int> push_count{0};
    std::atomic<int> pop_count{0};
    
    std::vector<std::thread> threads;
    
    // Launch producer threads
    for (int t = 0; t < num_threads / 2; ++t) {
        threads.emplace_back([&queue, &push_count, t, operations_per_thread]() {
            for (int i = 0; i < operations_per_thread; ++i) {
                Order order{static_cast<uint64_t>(t * operations_per_thread + i),
                           OrderType::BUY,
                           100.0 + i,
                           static_cast<uint32_t>(i + 1)};
                if (queue.push(order)) {
                    push_count.fetch_add(1);
                }
            }
        });
    }
    
    // Launch consumer threads
    for (int t = 0; t < num_threads / 2; ++t) {
        threads.emplace_back([&queue, &pop_count, operations_per_thread]() {
            Order order{0, OrderType::BUY, 0.0, 0};
            for (int i = 0; i < operations_per_thread; ++i) {
                if (queue.pop(order)) {
                    pop_count.fetch_add(1);
                }
            }
        });
    }
    
    // Wait for all threads
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Should have pushed and popped roughly the same number
    EXPECT_GT(push_count.load(), 0);
    EXPECT_GT(pop_count.load(), 0);
}

TEST_F(ConcurrencyTest, ConcurrentMemoryPoolAllocation) {
    const int num_threads = 4;
    const int allocations_per_thread = 1000;
    std::vector<std::thread> threads;
    std::atomic<int> total_allocations{0};
    
    // Launch threads that allocate from the same pool
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([this, &total_allocations, t, allocations_per_thread]() {
            std::vector<Order*> local_allocations;
            local_allocations.reserve(allocations_per_thread);
            
            for (int i = 0; i < allocations_per_thread; ++i) {
                Order* order = pool->allocate();
                EXPECT_NE(order, nullptr);
                
                // Initialize the order
                order->id = t * allocations_per_thread + i;
                order->type = OrderType::BUY;
                order->price = 100.0 + i;
                order->quantity = i + 1;
                
                local_allocations.push_back(order);
                total_allocations.fetch_add(1);
            }
            
            // Verify all allocations are unique within this thread
            for (size_t i = 0; i < local_allocations.size(); ++i) {
                for (size_t j = i + 1; j < local_allocations.size(); ++j) {
                    EXPECT_NE(local_allocations[i], local_allocations[j]);
                }
            }
        });
    }
    
    // Wait for all threads
    for (auto& thread : threads) {
        thread.join();
    }
    
    EXPECT_EQ(total_allocations.load(), num_threads * allocations_per_thread);
}

TEST_F(ConcurrencyTest, ProducerConsumerPattern) {
    LockFreeQueue<Order, 1000> queue;
    const int num_producers = 2;
    const int num_consumers = 2;
    const int orders_per_producer = 500;
    std::atomic<int> total_processed{0};
    
    std::vector<std::thread> threads;
    
    // Producer threads
    for (int p = 0; p < num_producers; ++p) {
        threads.emplace_back([&queue, p, orders_per_producer]() {
            for (int i = 0; i < orders_per_producer; ++i) {
                Order order{static_cast<uint64_t>(p * orders_per_producer + i),
                           (i % 2 == 0) ? OrderType::BUY : OrderType::SELL,
                           100.0 + i,
                           static_cast<uint32_t>(i + 1)};
                while (!queue.push(order)) {
                    std::this_thread::yield(); // Retry if queue is full
                }
            }
        });
    }
    
    // Consumer threads
    for (int c = 0; c < num_consumers; ++c) {
        threads.emplace_back([&queue, &total_processed, this]() {
            Order order{0, OrderType::BUY, 0.0, 0};
            int processed = 0;
            
            while (processed < 500) { // Each consumer processes 500 orders
                if (queue.pop(order)) {
                    ob.add_order(order);
                    processed++;
                    total_processed.fetch_add(1);
                } else {
                    std::this_thread::yield();
                }
            }
        });
    }
    
    // Wait for all threads
    for (auto& thread : threads) {
        thread.join();
    }
    
    EXPECT_EQ(total_processed.load(), num_producers * orders_per_producer);
    EXPECT_NO_THROW(ob.match_orders());
}

TEST_F(ConcurrencyTest, StressTestWithRandomOperations) {
    LockFreeQueue<Order, 1000> queue;
    const int num_threads = 6;
    const int operations_per_thread = 200;
    std::atomic<int> successful_operations{0};
    std::mt19937 rng(42);
    
    std::vector<std::thread> threads;
    
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([&queue, &successful_operations, t, operations_per_thread, &rng]() {
            std::uniform_int_distribution<int> op_dist(0, 1); // 0 = push, 1 = pop
            std::uniform_real_distribution<double> price_dist(100, 200);
            std::uniform_int_distribution<int> qty_dist(1, 100);
            
            for (int i = 0; i < operations_per_thread; ++i) {
                if (op_dist(rng) == 0) {
                    // Push operation
                    Order order{static_cast<uint64_t>(t * operations_per_thread + i),
                               OrderType::BUY,
                               price_dist(rng),
                               static_cast<uint32_t>(qty_dist(rng))};
                    if (queue.push(order)) {
                        successful_operations.fetch_add(1);
                    }
                } else {
                    // Pop operation
                    Order order{0, OrderType::BUY, 0.0, 0};
                    if (queue.pop(order)) {
                        successful_operations.fetch_add(1);
                    }
                }
            }
        });
    }
    
    // Wait for all threads
    for (auto& thread : threads) {
        thread.join();
    }
    
    EXPECT_GT(successful_operations.load(), 0);
}

TEST_F(ConcurrencyTest, OrderBookConcurrentMatch) {
    const int num_threads = 4;
    const int orders_per_thread = 100;
    
    // Pre-populate orderbook
    for (int i = 0; i < num_threads * orders_per_thread; ++i) {
        Order order{static_cast<uint64_t>(i),
                   (i % 2 == 0) ? OrderType::BUY : OrderType::SELL,
                   100.0 + (i % 100),
                   static_cast<uint32_t>((i % 50) + 1)};
        ob.add_order(order);
    }
    
    std::vector<std::thread> threads;
    
    // Launch threads that call match_orders concurrently
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([this]() {
            for (int i = 0; i < 10; ++i) {
                EXPECT_NO_THROW(ob.match_orders());
            }
        });
    }
    
    // Wait for all threads
    for (auto& thread : threads) {
        thread.join();
    }
}
