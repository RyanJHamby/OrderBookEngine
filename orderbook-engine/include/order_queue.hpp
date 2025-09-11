#pragma once
#include <atomic>
#include <vector>

template <typename T, size_t N>
class LockFreeQueue {
public:
    LockFreeQueue() : head(0), tail(0) {}

    bool push(const T& item) {
        size_t t = tail.load(std::memory_order_relaxed);
        size_t h = head.load(std::memory_order_acquire);
        if ((t + 1) % N == h) return false;
        buffer[t] = item;
        tail.store((t + 1) % N, std::memory_order_release);
        return true;
    }

    bool pop(T& item) {
        size_t h = head.load(std::memory_order_relaxed);
        size_t t = tail.load(std::memory_order_acquire);
        if (h == t) return false;
        item = buffer[h];
        head.store((h + 1) % N, std::memory_order_release);
        return true;
    }

private:
    T buffer[N];
    std::atomic<size_t> head;
    std::atomic<size_t> tail;
};


