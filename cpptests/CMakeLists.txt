cmake_minimum_required(VERSION 3.21)

project(cpptests CXX)

set(CMAKE_C_STANDARD 20)
set(CMAKE_CXX_STANDARD 20)

find_package(benchmark REQUIRED)

if(NOT benchmark_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.8.3
    )
    FetchContent_MakeAvailable(googlebenchmark)
endif()


add_executable(smartptr smartptr.cpp)
add_executable(lock_guard_tests lock_guard_tests.cpp)
add_executable(ref_tests ref_tests.cpp)
add_executable(move_semantics_benchmark move_semantics_benchmark.cpp)

target_link_libraries(move_semantics_benchmark benchmark::benchmark)
if(UNIX AND NOT APPLE)
    target_link_libraries(move_semantics_benchmark pthread)
endif()