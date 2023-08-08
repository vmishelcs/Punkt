cmake_minimum_required(VERSION 3.14)
set(CMAKE_C_COMPILER "C:/MinGW/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "C:/MinGW/bin/g++.exe")
set(CMAKE_CXX_STANDARD 11)
cmake_policy(SET CMP0135 NEW) # Removes developer warning
project(Punkt)

# No in-source build
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
	message(FATAL_ERROR "In-source builds are not allowed.")
endif("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")

add_compile_options(-g -Wall -Wextra -Werror -Wno-unused-parameter -Wuninitialized)

include_directories(${CMAKE_SOURCE_DIR})
include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

include(GoogleTest)
add_subdirectory(input_handler)
add_subdirectory(applications)
add_subdirectory(tests)
