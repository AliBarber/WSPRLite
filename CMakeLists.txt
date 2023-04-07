cmake_minimum_required(VERSION 3.14)
project(
  WSPREncoder
  VERSION 1.0
  LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 11)

## The library
add_library(WSPRLite INTERFACE)
target_include_directories(WSPRLite INTERFACE include/)
