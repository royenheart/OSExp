cmake_minimum_required(VERSION 3.16)

project(concurrent)

add_executable(Thread_mutex
    Threads/Thread_mutex.cpp
)

add_executable(rw_model_fair
    RW-Model/fair.cpp
)

add_executable(select_sem
    Concurrent_Select/select_sem.cpp
)

add_executable(select_mutex_cond
    Concurrent_Select/select_mutex_cond.cpp
)

target_link_libraries(Thread_mutex pthread)
target_link_libraries(rw_model_fair pthread)
target_link_libraries(select_sem pthread)
target_link_libraries(select_mutex_cond pthread)

file(COPY Concurrent_Select/test_select.sh DESTINATION .)