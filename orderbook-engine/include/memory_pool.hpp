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


