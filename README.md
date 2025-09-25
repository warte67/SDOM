# SDOM: Simple SDL Document Object Model API
[![License](https://img.shields.io/badge/license-ZLIB-blue.svg)](LICENSE)
[![Documentation](https://img.shields.io/badge/docs-Doxygen-blue)](https://warte67.github.io/SDOM/docs/html/)

Doxygen Documentation: https://warte67.github.io/SDOM/docs/html/

## Overview
The SDOM Framework is a modern C++23 library designed to provide a Document Object Model (DOM) abstraction for SDL3 applications. It enables developers to create and manage graphical user interfaces (GUIs) and interactive elements in a structured, hierarchical manner. The framework is ideal for game development, prototyping, and other cross-platform applications requiring a robust and extensible GUI system. SDOM is designed to be more data-driven, allowing GUI elements to be configured via `.json` files or through the use of initialization structures. This approach enables easier customization, dynamic UI generation, and better separation of logic and presentation.


## Features
- Simple, composable API for UI and document structures
- Modern C++23 codebase
- Minimal dependencies (SDL3, SDL3_image, SDL3_ttf)
- Designed for clarity, maintainability, and extensibility

## IDataObject: Data-Driven Base Type

`SDOM::IDataObject` is a flexible, data-driven base class designed for use in SDOM. It allows you to register property accessors (getters and setters) and commands that can be accessed and manipulated via JSON, enabling dynamic configuration, serialization, and behavior of objects.

### Key Features
- Register property accessors for any property using `registerProperty()`
- Get and set properties by name using `getProperty()` and `setProperty()`
- Supports chaining of setters for fluent configuration
- Register and invoke commands (actions) using `registerCommand()` and `command()`
- Designed for integration with JSON (using [nlohmann/json](https://github.com/nlohmann/json))


## Getting Started

### Prerequisites
- C++23 compatible compiler
- SDL3, SDL3_image, SDL3_ttf development libraries
- CMake 3.20 or newer

### Building the SDOM Library
```
git clone https://github.com/warte67/SDOM.git
cd SDOM
./rebuild # to build and install the SDOM library from this folder
```

### Running the Example
```
cd examples/test
./compile # to only build files which have changed
# or
./compile clean # to completely rebuild the library and the test example
./prog # to run the test example
```

## Project Structure
- `src/` — Core SDOM source code
- `include/SDOM/` — Public headers
- `examples/` — Example applications
- `docs/` — Documentation

## SDL3 Build Dependencies
see: https://github.com/libsdl-org/SDL/blob/main/docs/README-linux.md#build-dependencies

## Debian Build Dependencies
```
sudo apt-get install build-essential git make \
pkg-config cmake ninja-build gnome-desktop-testing libasound2-dev libpulse-dev \
libaudio-dev libjack-dev libsndio-dev libx11-dev libxext-dev \
libxrandr-dev libxcursor-dev libxfixes-dev libxi-dev libxss-dev libxtst-dev \
libxkbcommon-dev libdrm-dev libgbm-dev libgl1-mesa-dev libgles2-mesa-dev \
libegl1-mesa-dev libdbus-1-dev libibus-1.0-dev libudev-dev libfreetype-dev
```

## Arch Build Dependencies
```
sudo pacman -S alsa-lib cmake hidapi ibus jack libdecor libgl libpulse libusb libx11 libxcursor libxext libxinerama libxkbcommon libxrandr libxrender libxss libxtst mesa ninja pipewire sndio vulkan-driver vulkan-headers wayland wayland-protocols
```

## Freetype on Debian:
Freetype may not be installed in Debian by default.
```
sudo apt install libfreetype-dev
```

## Building and Installing SDL3_mixer, SDL3_image, and SDL_ttf
Clone the repositories:
```
git clone https://github.com/libsdl-org/SDL.git
git clone https://github.com/libsdl-org/SDL_mixer.git
git clone https://github.com/libsdl-org/SDL_image.git
git clone https://github.com/libsdl-org/SDL_ttf.git
```

## Build per SDL3:
```
git clone https://github.com/libsdl-org/SDL.git
cd SDL
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/.local
make -j$(nproc)
make install
```

## Build and install SDL3 libraries
```
# build SDL
cd SDL
mkdir build && cd build
cmake ..
make -j$(nproc) 
sudo make install

# build SDL_mixer
cd ../../SDL_mixer
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install

# build SDL_image
cd ../../SDL_image
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install

# build SDL_ttf
cd ../../SDL_ttf
mkdir build
cd build
cmake ..
make -j$(nproc)
sudo make install
```

## Lua and Sol2
Make sure lua is installed. 
Debian:
```
sudo apt update
sudo apt install liblua5.3-dev
```
or
```
sudo apt update
sudo apt install liblua5.4-dev
```
**Recommended:**  
Clone Sol2 into a dedicated third-party folder inside your project:
```
git clone https://github.com/ThePhD/sol2.git
```
**Note:** When integrating Sol2 and Lua, make sure to:
- Add the Sol2 include folder (e.g. include or third_party/sol2/include) to your compiler or IDE’s include path.
- Add the Lua include folder (e.g. lua5.3 or /usr/include/lua5.4) to your include path.
- Link to the Lua library (-llua5.3 or -llua5.4) in your build system (Makefile, CMake, etc.).
Example snippet in CMakeLists.txt:
```
include_directories(${CMAKE_SOURCE_DIR}/third_party/sol2/include)
include_directories(/usr/include/lua5.3)
find_library(LUA_LIB lua5.3)
target_link_libraries(your_target PRIVATE ${LUA_LIB})
```

## Documentation
Documentation is generated using [Doxygen](https://www.doxygen.nl/):
```bash
doxygen docs/Doxyfile
```
Output will be in the `docs/html/` directory.

## License
ZLIB License. See source files for details.

## Author
Jay Faries ([warte67](https://github.com/warte67))
