cmake_minimum_required(VERSION 3.18)
project(qt5_platform_maemo)
message(STATUS "Initiating compile using CMake ${CMAKE_VERSION}")

set(CMAKE_CXX_STANDARD 17)

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_SOURCE_DIR}/cmake")
include(FindCcache)

option(BUILD_EXAMPLE "Build the example application" OFF)

# generate globals.h (path to custom Qt plugins dir)
configure_file(
        ${CMAKE_SOURCE_DIR}/cmake/globals.h.cmake
        ${CMAKE_BINARY_DIR}/example/globals.h
        @ONLY # only @VARIABLE@ placeholders
)

# Find X11
if(UNIX AND NOT APPLE AND NOT ANDROID)
    if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_SKIP_RPATH ON)
    endif()

    find_package(X11 REQUIRED)
    message(STATUS "X11_FOUND = ${X11_FOUND}")
    message(STATUS "X11_INCLUDE_DIR = ${X11_INCLUDE_DIR}")
    message(STATUS "X11_LIBRARIES = ${X11_LIBRARIES}")

    include_directories(${X11_INCLUDE_DIR})
    link_directories(${X11_LIBRARIES})
    if(STATIC)
        find_library(XCB_LIBRARY xcb)
        message(STATUS "Found xcb library: ${XCB_LIBRARY}")
    endif()
endif()

# global compile defs
pkg_check_modules(LIBHILDON hildon-1)
if(DEFINED LIBHILDON_LIBRARIES)
    add_compile_definitions(MAEMO=1)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG=1)
endif()

# Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui Network Svg Xml X11Extras)
add_definitions(${QT_DEFINITIONS})

# targets
if(BUILD_EXAMPLE)
    add_subdirectory(example)
endif()

add_subdirectory(plugin)

