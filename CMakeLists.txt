cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(RiscV-Sym VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the include directory to the list of directories to search for header files
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add all source files in the src directory
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

# Add the executable, using the found source files
add_executable(${PROJECT_NAME} ${SOURCES})

# # Add the executable
# add_executable(${PROJECT_NAME}
#     src/Registers.cpp
#     src/RiscvCpu.cpp
#     src/main.cpp
#     # Add other source files here
# )

# Optionally, you can specify additional properties or link libraries here
# target_link_libraries(${PROJECT_NAME} some_library)