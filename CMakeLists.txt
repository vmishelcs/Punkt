cmake_minimum_required(VERSION 3.14)
set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")
set(CMAKE_CXX_STANDARD 20)
project(Punkt)

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

include_directories(${CMAKE_SOURCE_DIR})
include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

find_package(glog 0.6.0 REQUIRED)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

include(GoogleTest)
add_subdirectory(input_handler)
add_subdirectory(token)
add_subdirectory(scanner)
add_subdirectory(parse_node)
add_subdirectory(parser)
add_subdirectory(semantic_analyzer)
add_subdirectory(logging)
add_subdirectory(applications)
add_subdirectory(tests)
