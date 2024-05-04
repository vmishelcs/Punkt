cmake_minimum_required(VERSION 3.29)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING
	"Available build-types: Debug, Release")
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

# gtest
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.13.0 # release-1.13.0
)
FetchContent_MakeAvailable(googletest)
message(STATUS "Found GTest")

# LLVM 18.1.2
find_package(LLVM 18.1.2 REQUIRED CONFIG)
message(STATUS "Found LLVM")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
include_directories(${LLVM_INCLUDE_DIRS})
message(STATUS "LLVM include directories: ${LLVM_INCLUDE_DIRS}")
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

# Link ${llvm_libs} with targets
llvm_map_components_to_libnames(llvm_libs core support)

enable_testing()

add_subdirectory(applications)
add_subdirectory(code_generator)
add_subdirectory(logging)
add_subdirectory(input_handler)
add_subdirectory(parse_node)
add_subdirectory(parser)
add_subdirectory(scanner)
add_subdirectory(semantic_analyzer)
add_subdirectory(symbol_table)
add_subdirectory(tests)
add_subdirectory(token)
add_subdirectory(utilities)
