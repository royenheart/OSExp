cmake_minimum_required(VERSION 3.16)

project(epoll_test)

add_executable(epoll_server epoll_server.c)
add_executable(epoll_client epoll_client.c)
