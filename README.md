# Maemo Qt5 Keyboard Plug-in

Qt5 input module to handle Hildon events, incl. the virtual keyboard.

- `plugin/` - The plugin
- `example/` - Qt GUI test app 

Produces `libhiminputcontextplugin.so` and registers the input module under the key "him" for system-wide use.

## Development

`example/` contains a simple Qt GUI with input boxes. It uses some CMake and Qt magic to load the 
plugin from `build/qt/` in an isolated manner, allowing debugging (and ignore 
the system-wide installed Qt input plugins).

For example, `cmake -Bbuild -DBUILD_EXAMPLE=1 . && make -Cbuild -j4` will produce `build/bin/example` which  
uses the input plugin at `build/qt/platforminputcontexts/libhildonplatforminputcontextplugin.so`  
