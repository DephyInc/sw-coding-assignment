cmake_minimum_required(VERSION 3.5)

project(veprom VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES_COMMON
        vEPROM.cpp
        vEPROM_cmds.cpp
        vEPROM.h
        vEPROM_cmds.h
)

set(PROJECT_SOURCES_CLI
        ${PROJECT_SOURCES_COMMON}
        main_cli.cpp
)

set(PROJECT_SOURCES_GUI
        ${PROJECT_SOURCES_COMMON}
        main.cpp
        window.cpp
        window.h
)

add_executable(cli_veprom  ${PROJECT_SOURCES_CLI})
install(TARGETS cli_veprom DESTINATION bin)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
        add_executable(gui_veprom
        ${PROJECT_SOURCES_GUI}
    )

target_compile_options(gui_veprom PRIVATE -DGUI_VEPROM)
target_link_libraries(gui_veprom PRIVATE 
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Core
)

install(TARGETS gui_veprom DESTINATION bin)
endif()

set_target_properties(gui_veprom PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
)

include(GNUInstallDirs)
#install(TARGETS gui_veprom
#    BUNDLE DESTINATION .
#    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#)

