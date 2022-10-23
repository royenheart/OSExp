cmake_minimum_required(VERSION 3.16)

project(xtfs)

include_directories(src)

add_executable(format src/format.c)
add_executable(copy src/copy.c)
add_executable(read src/read.c)
add_executable(hello src/hello.c)

file(COPY xtfs.img DESTINATION test_file)
file(COPY src/hello.c DESTINATION test_file)