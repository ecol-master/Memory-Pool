########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 3.20)
project(mempool)

set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

include_directories(include/)

########################################################################
# Add headers to cmake project 
########################################################################

# List of directories that contain header files

add_subdirectory(src)

set(ALL_HEADER_DIRS
  include
)
