cmake_minimum_required(VERSION 3.20)
project(orderbook-engine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -pthread")
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64|i.86")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2")
endif()

include_directories(include)

file(GLOB SOURCES "src/*.cpp")
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

add_executable(orderbook src/main.cpp ${SOURCES})
add_executable(latency_benchmark ${SOURCES} benchmarks/latency_test.cpp)

# Add tests subdirectory
add_subdirectory(tests)


