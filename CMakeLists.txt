cmake_minimum_required(VERSION 3.16)

project(RiscvSim VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.5)

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE INSTRUCTION_SOURCES "${PROJECT_SOURCE_DIR}/src/instruction/*.cpp")

message(STATUS "SOURCES: ${SOURCES}")
message(STATUS "INSTRUCTION_SOURCES: ${INSTRUCTION_SOURCES}")

qt_add_executable(Riscv-Sim
    ${SOURCES} 
    ${INSTRUCTION_SOURCES}
)

qt_add_qml_module(Riscv-Sim
    URI RiscvSim
    VERSION 1.0
    QML_FILES
        Main.qml
        RESOURCES main.qrc
        MainPage.qml
        InstructionsPage.qml
        AboutPage.qml
        WelcomePage.qml
        InstructionCard.qml
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(Riscv-Sim PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Riscv-Sim
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(Riscv-Sim
    PRIVATE Qt6::Quick
)

include(GNUInstallDirs)
install(TARGETS Riscv-Sim
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
