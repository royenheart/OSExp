cmake_minimum_required(VERSION 3.16)

project(OSExp)

enable_testing()

add_subdirectory(functions_test)
add_subdirectory(threads)
add_subdirectory(processes)
add_subdirectory(xtfs)
add_subdirectory(concurrent)
add_subdirectory(epoll_test)