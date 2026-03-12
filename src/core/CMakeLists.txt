include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

include(FetchContent)

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.12.0
)
FetchContent_MakeAvailable(spdlog)

file(GLOB_RECURSE CORE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(riscv_core STATIC ${CORE_SOURCES})

target_include_directories(riscv_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(riscv_core PRIVATE spdlog::spdlog)

target_compile_features(riscv_core PUBLIC cxx_std_20)
