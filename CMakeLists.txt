cmake_minimum_required(VERSION 3.10)

project(RiscV-Sym VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE INSTRUCTION_SOURCES "${PROJECT_SOURCE_DIR}/src/instruction/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES} ${INSTRUCTION_SOURCES})

# # Add the executable
# add_executable(${PROJECT_NAME}
#     src/Registers.cpp
#     src/RiscvCpu.cpp
#     src/main.cpp
#     # Add other source files here
# )

# Optionally, you can specify additional properties or link libraries here
# target_link_libraries(${PROJECT_NAME} some_library)