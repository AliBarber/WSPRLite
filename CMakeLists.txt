cmake_minimum_required(VERSION 3.14)
project(
  WSPREncoder
  VERSION 1.0
  LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 11)

include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)
FetchContent_MakeAvailable(googletest)

## The library
add_library(WSPRLite INTERFACE)
target_include_directories(WSPRLite INTERFACE include/)

## Main demo 
add_executable(
	WSPR 
	src/WSPRLite.cpp
)
add_compile_definitions(
	WSPRLITE_STREAM
)
target_include_directories(
	WSPR PUBLIC
	include
)
target_compile_options(
	WSPR PRIVATE
	-Wreorder
	-Wsign-compare
	-Wunused-variable
)
## Tests
include(GoogleTest)

enable_testing()
add_executable(
	WSPR_test
	tests/WSPR_tests.cpp
)
add_compile_definitions(
	WSPR_test
	TEST_BUILD
)
target_link_libraries(
	WSPR_test
	gtest_main
)
message("${gtest_build_include_dirs}")
target_include_directories(
	WSPR_test PUBLIC
	include
)

gtest_discover_tests(WSPR_test)