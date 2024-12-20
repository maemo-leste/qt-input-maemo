# Maemo Qt5 Keyboard Plug-in

Qt5 input module to handle Hildon events, incl. the virtual keyboard.

- `plugin/` - The plugin
- `example/` - Qt GUI test app 

## Installation

The following build instructions will install this input module to `/usr/lib/x86_64-linux-gnu/qt5/plugins/platforminputcontexts/`.

```bash
cmake -Bbuild .
make -Cbuild -j4
sudo make install
```

After installation, simply run any Qt5 program with the `QT_IM_MODULE` environment variable:

```bash
QT_IM_MODULE=him ./some_qt_app
```

## Development

`example/` contains a simple Qt GUI with input boxes. It uses some CMake and Qt magic to load the 
plugin from `build/qt/` in an isolated manner, allowing debugging (ignoring 
the system-wide installed Qt input module).

```bash
cmake -Bbuild -DBUILD_EXAMPLE=1 . 
make -Cbuild -j4
``` 

Produces `build/bin/example` which uses the input module from the CMake build directory.  
