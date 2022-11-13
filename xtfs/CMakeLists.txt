cmake_minimum_required(VERSION 3.16)

project(xtfs)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_C_FLAGS_DEBUG "$ENV{CFLAGS} -DDEBUG -O0 -Wall -g -ggdb")
SET(CMAKE_C_FLAGS_RELEASE "$ENV{CFLAGS} -O3 -Wall")

option(FEATURE_JEMALLOC "Whether use jemalloc library to provide memory allocate service" ON)
option(TEST_BUILD "Build GTEST" ON)
option(BUILD_DOC "Build documentation" ON)

include_directories(src)

add_library(xtfs_manage SHARED src/xtfs_manage.c)
add_library(io SHARED src/io.c)

# Build Doxygen
if (BUILD_DOC)
    message(STATUS "Generate doxygen docs")
    include(build_doxygen.cmake)
    build_doxygen()
else (BUILD_DOC)
    message(STATUS "Not generate doxygen docs")
endif (BUILD_DOC)

# Jemalloc

if (FEATURE_JEMALLOC)
    message(STATUS "BUILD WITH JEMALLOC")
    target_link_libraries(xtfs_manage jemalloc)
else (FEATURE_JEMALLOC)
    message(STATUS "NOT BUILD WITH JEMALLOC")
endif ()

# GTEST

if (TEST_BUILD)
    find_package(GTest)
    if (GTest_FOUND)
        message(STATUS "Starting Bulild test using gtest")
        include_directories(${GTEST_INCLUDE_DIRS})

        add_executable(test_io src/gtests/test_io.cpp)

        target_link_libraries(test_io PUBLIC io ${GTEST_BOTH_LIBRARIES} pthread)

        enable_testing()
        add_test(NAME IO_BASIC_TEST COMMAND test_io)
    else (GTest_FOUND)
        message(STATUS "gtest lib not found")
    endif(GTest_FOUND)
else (TEST_BUILD)
    message(STATUS "NOT BUILD GTEST")
endif (TEST_BUILD)

add_executable(format src/format.c)
add_executable(copy src/copy.c)
add_executable(read src/read.c)
add_executable(rename src/rename.c)

target_link_libraries(copy PUBLIC io xtfs_manage)
target_link_libraries(read PUBLIC io xtfs_manage)
target_link_libraries(rename PUBLIC io xtfs_manage)

file(COPY xtfs.img DESTINATION test_file)
file(COPY hello.c DESTINATION test_file)
file(COPY gtest.img DESTINATION gtests)
file(COPY testbench.sh DESTINATION ${CMAKE_CURRENT_BINARY_DIR} FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE)