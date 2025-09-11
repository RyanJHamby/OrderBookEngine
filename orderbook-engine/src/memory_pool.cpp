// memory_pool.cpp
#include "memory_pool.hpp"
#include <cstdlib>

namespace ob {

void* MemoryPool::allocate() { return std::malloc(64); }
void MemoryPool::deallocate(void* ptr) noexcept { std::free(ptr); }

} // namespace ob


