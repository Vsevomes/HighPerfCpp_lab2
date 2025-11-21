cmake_minimum_required(VERSION 3.10)
project(SIMDProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -march=native")

find_package(benchmark QUIET)

if(NOT benchmark_FOUND)
    include(FetchContent)
    FetchContent_Declare(
      benchmark
      GIT_REPOSITORY https://github.com/google/benchmark.git
      GIT_TAG v1.34.1
    )
    FetchContent_MakeAvailable(benchmark)
endif()

set(SOURCES
    main.cpp
    src/substring_search.cpp
    src/bracket_balance.cpp
)

include_directories(include)

add_executable(simd_bench ${SOURCES})

if(benchmark_FOUND)
    target_link_libraries(simd_bench benchmark::benchmark pthread)
else()
    target_link_libraries(simd_bench benchmark pthread)
endif()
