cmake_minimum_required(VERSION 3.21)

project(cpptests CXX)

set(CMAKE_C_STANDARD 20)
set(CMAKE_CXX_STANDARD 20)

add_executable(smartptr smartptr.cpp)
add_executable(lock_guard_tests lock_guard_tests.cpp)