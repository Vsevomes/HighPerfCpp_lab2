cmake_minimum_required(VERSION 3.10)
project(simd_text_processing LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable AVX2
add_compile_options(-mavx2)

include_directories(include)

add_executable(simd_text_processing
    main.cpp
    src/substring_search.cpp
    src/bracket_balance.cpp
)

