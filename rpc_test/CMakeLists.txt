cmake_minimum_required(VERSION 3.21)

project(rpc_test CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

option(LINK_SO "Whether examples are linked dynamically" OFF)

find_package(GTest REQUIRED)
find_package(gflags REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Protobuf REQUIRED)
# generate protobuf files and then include & lib if there is any
# protobuf_generate_cpp(PROTO_SRC PROTO_HEADER helloworld.proto)
# include_directories(${CMAKE_CURRENT_BINARY_DIR})

# fetch BRPC
## Need to install its dependencies first:

## fetchcontent
include(FetchContent)
FetchContent_Declare(
  brpc
  GIT_REPOSITORY https://github.com/apache/brpc
  GIT_TAG c47ec71a0d30f6615af6b78329dfde2e69caba51 # version 1.13.0
  # SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/ext/brpc
)
## MakeAvailable acts like add_subdirectory, automatically adds the configure in the configure time, build in the build time
## Or it behaves like the above combination of FetchContent_GetProperties and FetchContent_Populate
# FetchContent_MakeAvailable(brpc)
## If we want to control the lib build behavior, we can use the FetchContent_GetProperties and FetchContent_Populate
FetchContent_GetProperties(brpc)
if(NOT brpc_POPULATED)
  # Fetch the content using previously declared details
  FetchContent_Populate(brpc)

  # Set custom variables, policies, etc.
  # ...
 
  # DO NOT Bring the populated content into the build
  # add_subdirectory(${depname_SOURCE_DIR} ${depname_BINARY_DIR})
endif()

## ExternalProject only run in the cmake build steps
# include(ExternalProject)
# ExternalProject_Add(
#   brpc
#   GIT_REPOSITORY https://github.com/apache/brpc
#   GIT_TAG        c47ec71a0d30f6615af6b78329dfde2e69caba51 # version 1.13.0
#   PREFIX ${CMAKE_CURRENT_BINARY_DIR}/ext/brpc
#   CMAKE_ARGS
#     -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
#     -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/ext/brpc_install
#   BUILD_ALWAYS OFF
#   STEP_TARGETS build install
# )

## configure brpc
message(STATUS "Configure brpc at configure time...")
execute_process(
  COMMAND ${CMAKE_COMMAND} -S ${brpc_SOURCE_DIR} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DBUILD_SHARED_LIBS=1 -DBUILD_STATIC_LIBS=1
  WORKING_DIRECTORY ${brpc_SOURCE_DIR}
  RESULT_VARIABLE configure_result
  OUTPUT_VARIABLE configure_output
  ERROR_VARIABLE configure_error
)

if(NOT configure_result EQUAL 0)
  message(FATAL_ERROR "Failed to configure brpc: ${configure_error}")
endif()

## build brpc
include(ProcessorCount)
ProcessorCount(CPU_CORES)
if(CPU_CORES GREATER 2)
  math(EXPR PARALLEL_JOBS "${CPU_CORES} - 2")
  if(PARALLEL_JOBS GREATER 14)
    set(PARALLEL_JOBS 14)
  endif()
else()
  set(PARALLEL_JOBS 1)
endif()
message(STATUS "Detected ${CPU_CORES} CPU cores, using ${PARALLEL_JOBS} parallel jobs")
message(STATUS "Building brpc at configure time...")
execute_process(
  COMMAND ${CMAKE_COMMAND} --build ${brpc_SOURCE_DIR} --parallel ${PARALLEL_JOBS}
  WORKING_DIRECTORY ${brpc_SOURCE_DIR}
  RESULT_VARIABLE build_result
  OUTPUT_VARIABLE build_output
  ERROR_VARIABLE build_error
)
  
if(NOT build_result EQUAL 0)
  message(FATAL_ERROR "Failed to build brpc: ${build_error}")
endif()
    
## install brpc
set(BRPC_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/ext/brpc_install)
message(STATUS "Installing brpc to ${BRPC_INSTALL_DIR}...")
execute_process(
  COMMAND ${CMAKE_COMMAND} --install ${brpc_SOURCE_DIR} --prefix ${BRPC_INSTALL_DIR}
  WORKING_DIRECTORY ${brpc_SOURCE_DIR}
  RESULT_VARIABLE install_result
  OUTPUT_VARIABLE install_output
  ERROR_VARIABLE install_error
)

if(NOT install_result EQUAL 0)
  message(FATAL_ERROR "Failed to install brpc")
endif()

set(CMAKE_PREFIX_PATH ${BRPC_INSTALL_DIR})

# find brpc include
find_path(BRPC_INCLUDE_PATH NAMES brpc/server.h)
if(NOT BRPC_INCLUDE_PATH)
  message(FATAL_ERROR "Fail to find brpc include path")
else()
  message(STATUS "Found brpc include path: ${BRPC_INCLUDE_PATH}")
  include_directories(${BRPC_INCLUDE_PATH})
endif()

# find brpc library
if(LINK_SO)
  find_library(BRPC_LIB NAMES brpc)
else()
  find_library(BRPC_LIB NAMES libbrpc.a brpc)
endif()
if(NOT BRPC_LIB)
  message(FATAL_ERROR "Fail to find brpc library")
else()
  message(STATUS "Found brpc lib: ${BRPC_LIB}")
endif()

# find gflags include and lib
find_path(GFLAGS_INCLUDE_PATH gflags/gflags.h)
find_library(GFLAGS_LIBRARY NAMES gflags libgflags)
if((NOT GFLAGS_INCLUDE_PATH) OR (NOT GFLAGS_LIBRARY))
  message(FATAL_ERROR "Fail to find gflags")
endif()
include_directories(${GFLAGS_INCLUDE_PATH})

set(DYNAMIC_LIB
  ${GFLAGS_LIBRARY}
  ${PROTOBUF_LIBRARIES}
  ${OPENSSL_SSL_LIBRARY}
  ${OPENSSL_CRYPTO_LIBRARY}
)

# set executables
add_executable(dbd_test dbd_test.cpp)

target_link_libraries(dbd_test PRIVATE ${BRPC_LIB} ${DYNAMIC_LIB})