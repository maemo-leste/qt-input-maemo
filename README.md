# Maemo Qt5 Keyboard Plug-in

Qt5 input plugin to handle Hildon events, incl. the virtual keyboard.

It produces `/usr/lib/x86_64-linux-gnu/qt5/plugins/platforminputcontexts/libhiminputcontextplugin.so` and 
registers a QT_IM module under the key "him"

## Development

`example/` contains a simple Qt GUI with input boxes. It uses some CMake and Qt magic to load the 
plugin from `build/qt/` in an isolated manner, allowing debugging (and ignore 
the system-wide installed Qt input plugins).

For example, `cmake -Bbuild -DBUILD_EXAMPLE=1 . && make -Cbuild -j4` will produce `build/bin/example` which  
uses the input plugin at `build/qt/platforminputcontexts/libhildonplatforminputcontextplugin.so`  
