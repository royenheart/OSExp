cmake_minimum_required(VERSION 3.16)

project(xtfs)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_C_FLAGS_DEBUG "$ENV{CFLAGS} -DDEBUG -O0 -Wall -g -ggdb")
SET(CMAKE_C_FLAGS_RELEASE "$ENV{CFLAGS} -O3 -Wall")

option(FEATURE_JEMALLOC "Whether use jamalloc library to provide memory allocate service" ON)

include_directories(src)
link_directories(src)

add_library(xtfs_manage SHARED src/xtfs_manage.c)
add_library(io SHARED src/io.c)

if (FEATURE_JEMALLOC)
target_link_libraries(xtfs_manage jemalloc)
endif()

add_executable(format src/format.c)
add_executable(copy src/copy.c)
add_executable(read src/read.c)

target_link_libraries(copy PUBLIC io xtfs_manage)
target_link_libraries(read PUBLIC io xtfs_manage)

file(COPY xtfs.img DESTINATION test_file)
file(COPY hello.c DESTINATION test_file)
