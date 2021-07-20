# MindForn CMake List
cmake_minimum_required (VERSION 3.8)

project (MindForn)

set(CMAKE_CXX_STANDARD 17)

file(GLOB MF_SOURCE "src/*")

add_executable(MindForn ${MF_SOURCE})