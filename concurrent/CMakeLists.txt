cmake_minimum_required(VERSION 3.16)

project(concurrent)

add_executable(Thread_mutex
    Threads/Thread_mutex.cpp
)

target_link_libraries(Thread_mutex pthread)
