cmake_minimum_required(VERSION 3.20)

# Find Google Test
find_package(GTest REQUIRED)

# Include directories
include_directories(../include)

# Test sources
set(TEST_SOURCES
    test_order.cpp
    test_orderbook.cpp
    test_order_queue.cpp
    test_memory_pool.cpp
)

# Create test executable
add_executable(orderbook_tests ${TEST_SOURCES})

# Link with Google Test and main library
target_link_libraries(orderbook_tests 
    GTest::GTest 
    GTest::Main
)

# Add main library sources
file(GLOB MAIN_SOURCES "../src/*.cpp")
target_sources(orderbook_tests PRIVATE ${MAIN_SOURCES})

# Enable testing
enable_testing()
add_test(NAME OrderBookTests COMMAND orderbook_tests)
