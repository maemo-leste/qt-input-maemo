file(GLOB SOURCE_FILES
        "*.h"
        "*.cpp"
        "lib/*.h"
        "lib/*.cpp"
        "xcb-maemo/*.h"
        "xcb-maemo/*.cpp"
)

add_executable(example main.cpp
        ${SOURCE_FILES}
        ${RESOURCES}
)


set_property(TARGET example PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

target_include_directories(example PUBLIC
        ${CMAKE_SOURCE_DIR}/src
        ${X11_INCLUDE_DIR}
        ${OPENSSL_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}/example
        ${Qt5Core_INCLUDE_DIRS}
        ${Qt5Widgets_INCLUDE_DIRS}
        ${Qt5Gui_INCLUDE_DIRS}
        ${Qt5Network_INCLUDE_DIRS}
        ${Qt5Svg_INCLUDE_DIRS}
        ${Qt5Xml_INCLUDE_DIRS}
        ${Qt5WebSockets_INCLUDE_DIRS}
        /usr/include/x86_64-linux-gnu/qt5/QtGui/5.15.2/QtGui/
)

target_compile_definitions(example
        PUBLIC
        ${Qt5Core_DEFINITIONS}
        ${Qt5Widgets_DEFINITIONS}
        ${Qt5Gui_DEFINITIONS}
        ${Qt5Network_DEFINITIONS}
        ${Qt5Svg_DEFINITIONS}
        ${Qt5Xml_DEFINITIONS}
        ${Qt5WebSockets_DEFINITIONS}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

target_link_libraries(example PUBLIC
        ${CMAKE_DL_LIBS}
        Qt5::Core
        Qt5::Widgets
        Qt5::Gui
        Qt5::X11Extras
        Qt5::Network
        Qt5::Xml
        #-lQt5XcbMaemoQpa
)

target_compile_definitions(example
        PRIVATE
        QT_PRIVATE_API
)

target_include_directories(example
        PRIVATE
        ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
)

add_dependencies(example plugin)

find_package(X11 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(XCB REQUIRED xcb)

target_compile_options(example PRIVATE ${XCB_CFLAGS_OTHER})
target_include_directories(example PRIVATE ${X11_INCLUDE_DIR})
target_link_libraries(example PRIVATE ${X11_LIBRARIES} ${XCB_LIBRARIES})

if(X11_FOUND)
    target_link_libraries(example PUBLIC ${X11_LIBRARIES})
endif()

install(TARGETS example
        DESTINATION ${CMAKE_INSTALL_PREFIX}
)
