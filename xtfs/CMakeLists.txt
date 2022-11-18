cmake_minimum_required(VERSION 3.16)

project(xtfs)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_C_FLAGS_DEBUG "$ENV{CFLAGS} -DDEBUG -O0 -Wall -g -ggdb")
SET(CMAKE_C_FLAGS_RELEASE "$ENV{CFLAGS} -O3 -Wall")

option(FEATURE_JEMALLOC "Whether use jemalloc library to provide memory allocate service" ON)
option(TEST_BUILD "Build GTEST" ON)
option(BUILD_DOC "Build documentation" ON)

include_directories(src)

add_library(xtfs_basic SHARED 
    src/xtfs_manage.c 
    src/xtfs_struct.c
    src/xtfs_check.c
    src/io.c
)

# Build Doxygen
if (BUILD_DOC)
    message(STATUS "Generate doxygen docs")
    include(${CMAKE_CURRENT_LIST_DIR}/cmake/build_doxygen.cmake)
    build_doxygen()
else (BUILD_DOC)
    message(STATUS "Not generate doxygen docs")
endif (BUILD_DOC)

# Jemalloc
if (FEATURE_JEMALLOC)
    message(STATUS "BUILD WITH JEMALLOC")
    target_link_libraries(xtfs_basic jemalloc)
else (FEATURE_JEMALLOC)
    message(STATUS "NOT BUILD WITH JEMALLOC")
endif ()

# GTEST
if (TEST_BUILD)
    find_package(GTest)
    if (GTest_FOUND)
        message(STATUS "Starting Bulild test using gtest")
        include_directories(${GTEST_INCLUDE_DIRS})

        add_executable(test_file_type src/gtests/test_file_type.cpp)
        add_executable(test_io src/gtests/test_io.cpp)

        target_link_libraries(test_io PUBLIC xtfs_basic ${GTEST_BOTH_LIBRARIES} pthread)
        target_link_libraries(test_file_type PUBLIC xtfs_basic ${GTEST_BOTH_LIBRARIES} pthread)

        enable_testing()
        add_test(NAME IO_BASIC_TEST COMMAND test_io)
        add_test(NAME FILE_TYPE_TEST COMMAND test_file_type)
    else (GTest_FOUND)
        message(STATUS "gtest lib not found")
    endif(GTest_FOUND)
else (TEST_BUILD)
    message(STATUS "NOT BUILD GTEST")
endif (TEST_BUILD)

link_libraries(xtfs_basic)

add_executable(format src/format.c)
add_executable(copy src/copy.c)
add_executable(read src/read.c)
add_executable(rename src/rename.c)
add_executable(delete src/delete.c)
add_executable(rewrite src/rewrite.c)
add_executable(cipher src/features/cipher.c)
add_executable(decrypt src/features/decrypt.c)
add_executable(HuffmanUnzip src/features/HuffmanUnzip.cpp)
add_executable(HuffmanZip src/features/HuffmanZip.cpp)

file(COPY tests/xtfs.img DESTINATION tests)
file(COPY tests/hello.c DESTINATION tests)
file(COPY tests/hello.cpp DESTINATION tests)
file(COPY tests/hello.md DESTINATION tests)
file(COPY tests/short_hello.md DESTINATION tests)
file(COPY tests/long_hello.md DESTINATION tests)
file(COPY tests/gtest.img DESTINATION gtests)
file(COPY tests/testbench.sh DESTINATION ${CMAKE_CURRENT_BINARY_DIR} FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE)