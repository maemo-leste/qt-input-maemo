cmake_minimum_required(VERSION 3.18)
project(qt5_platform_maemo)
message(STATUS "Initiating compile using CMake ${CMAKE_VERSION}")

set(CMAKE_CXX_STANDARD 20)

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_SOURCE_DIR}/cmake")
include(FindCcache)

option(BUILD_EXAMPLE "Build the example application" OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
            "Default build type: Release" FORCE)
endif()

find_package(PkgConfig REQUIRED)

# These 2 are needed because we include `hildon-im-protocol.h` for some enums
pkg_check_modules(GTK REQUIRED gtk+-2.0)
pkg_check_modules(HIM3 REQUIRED hildon-input-method-framework-3.0)

find_package(X11 REQUIRED)
message(STATUS "X11_FOUND = ${X11_FOUND}")
message(STATUS "X11_INCLUDE_DIR = ${X11_INCLUDE_DIR}")
message(STATUS "X11_LIBRARIES = ${X11_LIBRARIES}")

include_directories(${X11_INCLUDE_DIR})
link_directories(${X11_LIBRARIES})

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

