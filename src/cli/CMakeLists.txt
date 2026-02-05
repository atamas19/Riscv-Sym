cmake_minimum_required(VERSION 3.16)

file(GLOB_RECURSE CLI_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable(riscv_cli
    ${CLI_SOURCES}
)

target_link_libraries(riscv_cli PRIVATE
    riscv_core
)
