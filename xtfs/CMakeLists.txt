cmake_minimum_required(VERSION 3.16)

project(xtfs)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_C_FLAGS_DEBUG "$ENV{CFLAGS} -DDEBUG -O0 -Wall -g -ggdb")
SET(CMAKE_C_FLAGS_RELEASE "$ENV{CFLAGS} -O3 -Wall")

option(FEATURE_JEMALLOC "Whether use jemalloc library to provide memory allocate service" ON)
option(TEST_BUILD "Build GTEST" ON)
option(BUILD_DOC "Build documentation" ON)

include_directories(src src/lex)

add_library(xtfs_basic SHARED 
    src/xtfs_manage.c 
    src/xtfs_struct.c
    src/xtfs_check.c
    src/io.c
    src/lex/lex.yy.c
)

execute_process(
  COMMAND dot -V
  RESULT_VARIABLE dot_result
  OUTPUT_VARIABLE dot_output
  ERROR_VARIABLE dot_error
)

# Build Doxygen
if (BUILD_DOC AND (dot_result EQUAL 0))
    message(STATUS "Generate doxygen docs")
    include(${CMAKE_CURRENT_LIST_DIR}/cmake/build_doxygen.cmake)
    build_doxygen()
elseif(NOT dot_result EQUAL 0)
    message(WARNING "Doxygen docs generation failed due to lack of graphviz: ${dot_error}")
    message(WARNING "Install graphviz you can: sudo apt install -y graphviz / sudo dnf install -y graphviz")
else()
    message(WARNING "Doxygen docs generation disabled")
endif()

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
        add_executable(test_folder_lex src/gtests/test_folder_lex.cpp)

        target_link_libraries(test_io PUBLIC xtfs_basic ${GTEST_BOTH_LIBRARIES} pthread)
        target_link_libraries(test_file_type PUBLIC xtfs_basic ${GTEST_BOTH_LIBRARIES} pthread)
        target_link_libraries(test_folder_lex PUBLIC xtfs_basic ${GTEST_BOTH_LIBRARIES} pthread)

        enable_testing()
        add_test(NAME IO_BASIC_TEST COMMAND test_io)
        add_test(NAME FILE_TYPE_TEST COMMAND test_file_type)
        add_test(NAME FOLDER_LEX_TEST COMMAND test_folder_lex)
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
add_executable(mv src/mv.c)
add_executable(delete src/delete.c)
add_executable(rewrite src/rewrite.c)
add_executable(mkdir src/mkdir.c)
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
file(COPY tests/bigFile.md DESTINATION tests)
file(COPY tests/gtest.img DESTINATION gtests)