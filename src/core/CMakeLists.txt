include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

file(GLOB_RECURSE CORE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(riscv_core STATIC ${CORE_SOURCES})

target_include_directories(riscv_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(riscv_core PUBLIC cxx_std_20)
