// memory_pool.hpp
#pragma once

#include <cstddef>
#include <cstdint>

namespace ob {

class MemoryPool {
public:
    explicit MemoryPool(std::size_t /*blockSize*/, std::size_t /*blockCount*/) {}
    ~MemoryPool() = default;

    void* allocate();
    void deallocate(void* ptr) noexcept;
};

} // namespace ob



#include <vector>

template<typename T>
class ThreadLocalPool {
public:
    ThreadLocalPool(size_t size = 1024*1024) {
        pool.reserve(size);
    }

    T* allocate() {
        if (pool_index < pool.size()) return &pool[pool_index++];
        pool.emplace_back();
        return &pool[pool_index++];
    }

private:
    std::vector<T> pool;
    size_t pool_index = 0;
};

