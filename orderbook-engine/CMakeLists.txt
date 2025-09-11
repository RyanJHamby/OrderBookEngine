cmake_minimum_required(VERSION 3.20)

project(orderbook-engine
  VERSION 0.1.0
  DESCRIPTION "Low-latency C++ Order Book"
  LANGUAGES CXX)

# Options
option(ORDERBOOK_ENABLE_LTO "Enable link-time optimization" ON)
option(ORDERBOOK_BUILD_BENCHMARKS "Build benchmark binaries" ON)

# Default to Release
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# C++ standard and compile flags
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Baseline compile options
add_compile_options(
  $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:-Wall -Wextra -Wpedantic>
  $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:-Wconversion -Wshadow -Wdouble-promotion>
  $<$<CXX_COMPILER_ID:Clang,AppleClang>:-Wshorten-64-to-32>
  $<$<CONFIG:Release>:-O3 -DNDEBUG>
  $<$<AND:$<CXX_COMPILER_ID:Clang,AppleClang,GNU>,$<CONFIG:Release>>:-march=native>
)

# LTO
if(ORDERBOOK_ENABLE_LTO)
  include(CheckIPOSupported)
  check_ipo_supported(RESULT lto_supported OUTPUT lto_output)
  if(lto_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
  endif()
endif()

# Include dirs
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Library
file(GLOB ORDERBOOK_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/order.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/orderbook.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/memory_pool.cpp
)

add_library(orderbook STATIC ${ORDERBOOK_SOURCES})
set_target_properties(orderbook PROPERTIES OUTPUT_NAME orderbook)

# Main executable
add_executable(orderbook-engine ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

# Link
target_link_libraries(orderbook-engine PRIVATE orderbook)

# Benchmarks
if(ORDERBOOK_BUILD_BENCHMARKS)
  add_executable(latency_benchmark ${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/latency_test.cpp)
  target_link_libraries(latency_benchmark PRIVATE orderbook)
endif()

# Install (optional basic targets)
include(GNUInstallDirs)
install(TARGETS orderbook orderbook-engine
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/orderbook)

# Testing placeholder (can hook into ctest later)


