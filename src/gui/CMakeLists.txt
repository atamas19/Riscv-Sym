cmake_minimum_required(VERSION 3.16)

find_package(Qt6 REQUIRED COMPONENTS Quick Core Gui)

qt_standard_project_setup(REQUIRES 6.5)

file(GLOB_RECURSE GUI_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

qt_add_executable(riscv_viz
    ${GUI_SOURCES}
)

qt_add_qml_module(riscv_viz
    URI RiscvSim
    VERSION 1.0
    QML_FILES
        Main.qml
        MainPage.qml
        InstructionsPage.qml
        AboutPage.qml
        WelcomePage.qml
        InstructionCard.qml
    RESOURCES
        main.qrc
)

set_target_properties(riscv_viz PROPERTIES
    # MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Riscv-Viz
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(riscv_viz PRIVATE
    Qt6::Quick
    riscv_core
)

include(GNUInstallDirs)
install(TARGETS riscv_viz
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
