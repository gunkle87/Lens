cmake_minimum_required(VERSION 3.20)

project(lens C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# v0 bootstrap:
# - Source targets are intentionally added per task directives.
# - Keep root build setup minimal and deterministic.
