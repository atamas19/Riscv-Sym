cmake_minimum_required(VERSION 3.16)

find_package(GTest 1.14.0 QUIET)
if(NOT GTest_FOUND)
    message(STATUS "GTest (>=1.14.0) not found on system. Fetching from GitHub...")

    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
else()
    message(STATUS "Found GTest installed on system.")
endif()

add_executable(riscv_tests
    InstructionTests.cpp
)

target_link_libraries(riscv_tests PRIVATE
    riscv_core
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(riscv_tests)
