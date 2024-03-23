cmake_minimum_required(VERSION 3.24.0)
set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(Punkt)

include_directories(${CMAKE_SOURCE_DIR})

# No in-source build
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
	message(FATAL_ERROR "In-source builds are not allowed.")
endif("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")

add_compile_options(
	-g
	-Wall
	-Wextra
	-Werror
	-Wno-unused-parameter
	-Wno-deprecated-copy
	-Wuninitialized
)

include(FetchContent)

# GoogleTest 1.14.0
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)
message(STATUS "Found GoogleTest")

# Glog 0.7.0
set(WITH_GFLAGS OFF)
FetchContent_Declare(
	glog
	URL https://github.com/google/glog/archive/refs/tags/v0.7.0.zip
)
FetchContent_MakeAvailable(glog)
message(STATUS "Found GLog")

# LLVM 18.1.2
find_package(LLVM 18.1.2 REQUIRED CONFIG)
message(STATUS "Found LLVM")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
include_directories(${LLVM_INCLUDE_DIRS})
message(STATUS "LLVM include directories: ${LLVM_INCLUDE_DIRS}")
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

# Link ${llvm_libs} with targets
llvm_map_components_to_libnames(llvm_libs core)

enable_testing()

add_subdirectory(applications)
add_subdirectory(logging)
add_subdirectory(input_handler)
add_subdirectory(parse_node)
add_subdirectory(parser)
add_subdirectory(scanner)
add_subdirectory(semantic_analyzer)
add_subdirectory(symbol_table)
add_subdirectory(tests)
add_subdirectory(token)
