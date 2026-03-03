cmake_minimum_required(VERSION 3.16)

project(RiscV-Sim VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_GUI "Build the GUI interface" OFF)
option(ENABLE_CLI "Build the CLI interface" ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(src/core)
if(ENABLE_GUI)
    add_subdirectory(src/gui)
endif()

if(ENABLE_CLI)
    add_subdirectory(src/cli)
endif()
