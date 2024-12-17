file(GLOB SOURCE_FILES
        "*.h"
        "*.cpp"
        "lib/*.h"
        "lib/*.cpp"
        "xcb-maemo/*.h"
        "xcb-maemo/*.cpp"
)

add_library(plugin MODULE ${SOURCE_FILES})

set_target_properties(plugin PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qt/platforminputcontexts/"
        OUTPUT_NAME "himplatforminputcontextplugin"
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(plugin PRIVATE DEBUG=1)
endif()

target_include_directories(plugin PUBLIC ${Qt5Gui_PRIVATE_INCLUDE_DIRS})
target_include_directories(plugin PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(plugin
        Qt5::Core
        Qt5::Widgets
        Qt5::Gui
        Qt5::GuiPrivate # for qpa
        Qt5::X11Extras
        -lQt5XcbMaemoQpa
)

# @TODO: /usr/lib/x86_64-linux-gnu/qt5/plugins/platforminputcontexts/libhiminputcontextplugin.so
install(TARGETS plugin LIBRARY DESTINATION platforminputcontexts)
