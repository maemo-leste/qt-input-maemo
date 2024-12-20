file(GLOB SOURCE_FILES
    "*.h"
    "*.cpp"
)

add_library(plugin MODULE ${SOURCE_FILES})

set_target_properties(plugin PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qt/platforminputcontexts/"
        OUTPUT_NAME "himplatforminputcontextplugin"
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(plugin PRIVATE DEBUG=1)
endif()

# This definition is needed to stop a declaration conflict with
# 'HILDON_GTK_INPUT_MODE_ALPHA' (and others) both defined in
# 'hildon-im-protocol.h' and 'gtkenums.h'.
# We can remove this if we get rid of the !defined(MAEMO_CHANGES)
# inside 'hildon-im-protocol.h'
target_compile_definitions(plugin PRIVATE MAEMO_CHANGES=1)

target_include_directories(plugin PRIVATE
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${HIM3_INCLUDE_DIRS}
    ${GTK_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(plugin
        Qt5::Core
        Qt5::Widgets
        Qt5::Gui
        Qt5::GuiPrivate # for qpa
        Qt5::X11Extras
        ${HIM3_LIBRARIES}
        ${GTK_LIBRARIES}
)


# install target with dest: /usr/lib/x86_64-linux-gnu/qt5/plugins/platforminputcontexts/
# find this path with qmake
execute_process(
    COMMAND qmake -query QT_INSTALL_PLUGINS
    OUTPUT_VARIABLE QT_PLUGINS_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(PLUGIN_INSTALL_TARGET "${QT_PLUGINS_DIR}/platforminputcontexts/")
message(STATUS "install target: ${PLUGIN_INSTALL_TARGET}")
install(TARGETS plugin LIBRARY DESTINATION ${PLUGIN_INSTALL_TARGET})

# summary
message(STATUS "[+] Build type: ${CMAKE_BUILD_TYPE}")
if(BUILD_EXAMPLE)
    message(STATUS "[+] Build example: yes")
else()
    message(STATUS "[+] Build example: no")
endif()
