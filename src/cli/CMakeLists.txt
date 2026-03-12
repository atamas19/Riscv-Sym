cmake_minimum_required(VERSION 3.16)

include(FetchContent)

FetchContent_Declare(
  cli11
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
  GIT_TAG        v2.5.0
)
FetchContent_MakeAvailable(cli11)

file(GLOB_RECURSE CLI_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable(riscv_cli
    ${CLI_SOURCES}
)

target_link_libraries(riscv_cli PRIVATE
    riscv_core
    CLI11::CLI11
    spdlog::spdlog
)
