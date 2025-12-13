cmake_minimum_required(VERSION 3.16)

project(RiscV-Sim VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(src/core)

add_subdirectory(src/gui)
