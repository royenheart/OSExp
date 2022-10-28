cmake_minimum_required(VERSION 3.16)

project(concurrent)

add_executable(Thread_mutex
    Threads/Thread_mutex.cpp
)

add_executable(rw_model_fair
    RW-Model/fair.cpp
)

target_link_libraries(Thread_mutex pthread)
target_link_libraries(rw_model_fair pthread)