cmake_minimum_required(VERSION 3.26)
project(
  "WSPRLite"
  VERSION 1.1
  LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 11)

add_library(WSPRLite INTERFACE)
target_include_directories(WSPRLite INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)