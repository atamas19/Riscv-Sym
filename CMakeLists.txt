cmake_minimum_required(VERSION 3.16)

project(RiscV-Sim VERSION 0.1 LANGUAGES CXX)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(WITH_GUI "Build the GUI interface" OFF)
option(WITH_CLI "Build the CLI interface" ON)
option(WITH_TESTS "Build the test suite" ON)

find_package(spdlog 1.12.0 QUIET)
if(NOT spdlog_FOUND)
    message(STATUS "spdlog (>=1.12.0) not found on system. Fetching from GitHub...")
    FetchContent_Declare(
      spdlog
      GIT_REPOSITORY https://github.com/gabime/spdlog.git
      GIT_TAG        v1.12.0
    )
    set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "Disable spdlog tests" FORCE)
    set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "Disable spdlog examples" FORCE)
    set(SPDLOG_BUILD_BENCH OFF CACHE BOOL "Disable spdlog benchmarks" FORCE)
    FetchContent_MakeAvailable(spdlog)
else()
    message(STATUS "Found spdlog installed on system.")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(src/core)
if(WITH_GUI)
    add_subdirectory(src/gui)
endif()

if(WITH_CLI)
    find_package(CLI11 2.5.0 QUIET)
    if(NOT CLI11_FOUND)
        message(STATUS "CLI11 (>=2.5.0) not found on system. Fetching from GitHub...")
        FetchContent_Declare(
        cli11
        GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
        GIT_TAG        v2.5.0
        )
        set(CLI11_BUILD_TESTS OFF CACHE BOOL "Disable CLI11 tests" FORCE)
        set(CLI11_BUILD_EXAMPLES OFF CACHE BOOL "Disable CLI11 examples" FORCE)
        set(CLI11_BUILD_DOCS OFF CACHE BOOL "Disable CLI11 docs" FORCE)
        FetchContent_MakeAvailable(cli11)
    else()
        message(STATUS "Found CLI11 installed on system.")
    endif()
    add_subdirectory(src/cli)
endif()

if(WITH_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
