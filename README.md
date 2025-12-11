# SDOM: Simple SDL Document Object Model API
[![License](https://img.shields.io/badge/license-ZLIB-blue.svg)](LICENSE)
[![Documentation](https://img.shields.io/badge/docs-Doxygen-blue)](https://warte67.github.io/SDOM/)
[![Wiki](https://img.shields.io/badge/github-wiki-181717?logo=github)](https://github.com/warte67/SDOM/wiki)
[![Status](https://img.shields.io/badge/status-pre--alpha-orange.svg)]()

<!-- BEGIN_VERSION_BLOCK -->
**SDOM 0.5.274 (early pre-alpha)**  
**Build Date:** 2025-12-11_10:26:00  
**Platform:** Linux-x86_64  
**Compiler:** g++ (GCC) 15.2.1 20251112
<!-- END_VERSION_BLOCK -->

- GitHub Wiki: https://github.com/warte67/SDOM/wiki

> ⚠️ **Development Status:** SDOM is a **very early pre-alpha prototype**.  
> It is not feature-complete, APIs are unstable, and design work is ongoing.  
> This repository is public for collaboration and early feedback.

---

### Screenshot of the Test Harness application:
![Test Harness](https://raw.githubusercontent.com/warte67/SDOM/refs/heads/master/examples/test/docs/diagrams/SDOM_Test_Harness.png)

## Overview

**SDOM** (Software Display Object Model) is a C++23 framework that brings a **structured, DOM-like architecture** to SDL3-based rendering and input. It treats your UI and scene elements as a **tree of display objects** — each with defined properties, bounds, parent/child relationships, and event semantics — enabling declarative UI composition, deterministic updates, and clean separation between logic and rendering.

Ideal for **game UI**, **custom editors**, **visual tools**, and any application wanting a modern, high-level interface on top of SDL3.

SDOM is both **cross-platform** and **cross-language by design**. It exposes a safe, pointer-free **C ABI** that allows any language capable of calling C — Rust, D, Zig, Python, C#, Swift, etc. — to use the engine *without wrappers*. Higher-level bindings, such as Lua today and Python in the future, sit naturally on top of this ABI for scripted interaction and live editing when desired.

SDOM’s language-neutrality is driven by a **data-driven reflection system**. Every type, property, and command registered at runtime becomes metadata that the build system can use to **automatically generate bindings** for external languages: C API headers, Lua tables, future Rust/D/Python modules, and more — all without handwritten glue code. Adding support for a new language is simply adding a **new binding-generator plugin**.

At its core, SDOM is a **JSON-native runtime**. All data — properties, event payloads, configuration — flows through a single universal value type: `Variant`. This enables **live editing** and script-driven behavior without recompiling. And all dynamic systems are **optional**: a pure C++ application can use SDOM with *zero* scripting overhead, while dynamic languages can plug in to extend or prototype as needed.

SDOM is engineered for **extensibility without bloat**. Optional modules — runtime editor, Lua integration, addon/plugin support — can be enabled or omitted, giving developers the ability to scale from a minimal embedding to a fully featured, interactive toolchain. The same core reflection engine powers all future systems: inspectors, interactive UI editing, metadata-driven workflows, and custom type registration.

**Note:** This is still a very early pre-alpha version.  All APIs are subject to change.  

## Features
- Simple, composable API for UI and document structures
- Modern C++23 codebase
- Minimal dependencies (SDL3, SDL3_image, SDL3_ttf)
- Designed for clarity, maintainability, and extensibility

# Performance Highlights (December 8, 2025):

> **Designed for modern workflows — runs on 2013 laptops.**

### 🧪 Real-World Test Environment
- Hardware: ~12-year-old mid-tier PC (no GPU reliance beyond SDL3)
- Build: Debug, no compiler tuning, all safety checks enabled
- Scenario: Full SDOM UI sample with multiple widgets & event routing

### 📈 Results

| Metric | Result | Meaning |
|---|:---:|---|
| Unit Tests | **82 / 82 Passed** | Stable core API and behavior |
| Total Frame Time | **~201 µs** | ~4,900 FPS theoretical headroom |
| Top Widget Update Cost | **0.6–0.8 µs** | Negligible per-node CPU use |
| Top Render Cost | **1.1–1.8 µs** | Minimal pixel batching overhead |
| Memory Leaks | **0 bytes leaked** | Safe for long-running editor/tool |

> **Even micro-widgets** (radio/check buttons, labels)  
> stay under **1 µs update** and **2 µs render** cost — *in debug builds*.

### 🧩 Optional-Feature Architecture — Why It Matters

| Feature | Status | Cost if Disabled |
|---|---|---|
| Live Editor | Planned | **0 overhead** |
| Lua Scripting | Optional | **0 overhead** |
| Variant Legends | Planned | **0 overhead** |
| Physics / Networking | Planned | **0 overhead** |

SDOM ships **only** the systems a project needs — no “always-on” bloat.

### 🛡️ Runtime Safety
- Threaded load tests pass
- Snapshot-safe Variant usage
- Deterministic error messaging
- Long-run allocation safety verified

---

> **Conclusion:**  
> SDOM isn’t just flexible — it’s **fast**.  
> The foundation is strong enough to power the future live editor  
> *without sacrificing performance on modest hardware.*


---

## Getting Started

### Prerequisites
- C++23 compatible compiler
- SDL3, SDL3_image, SDL3_ttf development libraries
- CMake 3.20 or newer

### ⚠️ Before You Build

> **Important:** The SDOM API depends on external libraries that **must be installed before building**.  
> Ensure you have built and installed **SDL3**, **SDL3_image**, **SDL3_ttf**, and (eventually) **SDL3_mixer**,  
> as well as **Lua** and **Sol2**, *before running* `./rebuild`.

If you haven’t installed these yet, see the **“SDL3 Build Dependencies”** and **“Lua Installation”** sections below.

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

### Notable recent changes
- Symbol rename: DisplayObject -> DisplayHandle and AssetObject -> AssetHandle across the codebase and Lua scripts (Oct 2025).
  - The code and examples have been updated; some diagrams and docs still reference the old names and will be updated in a follow-up.
  - Lua compatibility: Lua userdata/type names remain stable where necessary; check the README/changelog for per-release compatibility notes.

### Migration notes
- If you maintain scripts that referenced the old names, search & replace DisplayObject / AssetObject -> DisplayHandle / AssetHandle.
- The repository includes transitional aliases during migration for staged rollouts (if present); prefer the new names in new code.
- Binding changes must go through data registration and the generator; do not hand-edit generated C API outputs.

## Project Structure
- `src/` — Core SDOM source code
- `include/SDOM/` — Public headers
- `examples/` — Example applications
- `docs/` — Documentation

---
# Lua Integration and Requirements

SDOM is designed to be **data-driven** and includes built-in support for scripting and configuration through **Lua (via Sol2)**.

### Why Lua?
Lua enables dynamic creation, modification, and serialization of DOM objects at runtime — letting you define UI layouts, animation sequences, and event behavior without recompiling. Lua scripts can access most of SDOM’s API, and future releases will expand this integration even further.

### Lua Installation

**Debian / Ubuntu:**
```bash
sudo apt update
sudo apt install liblua5.4-dev
```
**Arch Linux:**
```bash
sudo pacman -S lua
```
**Fedora / RHEL / CentOS Stream / Alma / Rocky:**
```bash
sudo dnf install lua-devel
```
**macOS:**
```bash
brew install lua
```
**Windows:**
Download and install the latest Lua for Windows from [Lua.org](https://www.lua.org/download.html).

---
# SDL3 Build Dependencies
See the official SDL documentation:  
🔗 https://github.com/libsdl-org/SDL/blob/main/docs/README-linux.md#build-dependencies

---
### 🐧 Debian / Ubuntu
```bash
sudo apt-get install build-essential git make \
pkg-config cmake ninja-build gnome-desktop-testing libasound2-dev libpulse-dev \
libaudio-dev libjack-dev libsndio-dev libx11-dev libxext-dev \
libxrandr-dev libxcursor-dev libxfixes-dev libxi-dev libxss-dev libxtst-dev \
libxkbcommon-dev libdrm-dev libgbm-dev libgl1-mesa-dev libgles2-mesa-dev \
libegl1-mesa-dev libdbus-1-dev libibus-1.0-dev libudev-dev libfreetype-dev
```

## 🐧 Arch Linux
```bash
sudo pacman -S alsa-lib cmake hidapi ibus jack libdecor libgl libpulse libusb \
libx11 libxcursor libxext libxinerama libxkbcommon libxrandr libxrender \
libxss libxtst mesa ninja pipewire sndio vulkan-driver vulkan-headers \
wayland wayland-protocols freetype2
```

## 🐧 Fedora / RHEL / Alma / Rocky
```bash
sudo dnf install @development-tools cmake ninja-build git \
SDL3-devel SDL3_image-devel SDL3_ttf-devel SDL3_mixer-devel \
alsa-lib-devel pulseaudio-libs-devel libX11-devel libXext-devel \
libXrandr-devel libXcursor-devel libXi-devel libXss-devel libXtst-devel \
libxkbcommon-devel mesa-libGL-devel mesa-libEGL-devel dbus-devel \
freetype-devel
```

## 🍎 macOS
```bash
brew install sdl3 sdl3_image sdl3_ttf sdl3_mixer cmake ninja freetype
```

## 🪟 Windows
### Option 1:
Download and install the latest SDL3 development libraries from [SDL.org](https://www.libsdl.org/download-2.0.php).
### Option 2: MSYS2 (MinGW / UCRT64)
```bash
pacman -S mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-ninja \
          mingw-w64-ucrt-x86_64-SDL3 mingw-w64-ucrt-x86_64-SDL3_image \
          mingw-w64-ucrt-x86_64-SDL3_ttf mingw-w64-ucrt-x86_64-SDL3_mixer \
          mingw-w64-ucrt-x86_64-freetype
```
### Option 3: vcpkg (Visual Studio / CMake Integration)
```bash
vcpkg install sdl3 sdl3-image sdl3-ttf sdl3-mixer freetype
```
Then integrate vcpkg with CMake:
```bash
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
```
---
# Building and Installing SDL3_mixer, SDL3_image, and SDL_ttf
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

## Sol2 (recommended)
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

Additional docs:
- Overall architecture diagram: docs/architecture_overview.md

### Exporting Mermaid Diagrams to PNG/SVG (for Doxygen)

We embed Mermaid diagrams in Markdown. To export them as images for inclusion in generated HTML (or for environments without Mermaid support), install mermaid-cli and run the helper script:

Debian:
```
sudo apt install nodejs npm
```

Install Mermaid CLI globally:
```bash
npm install -g @mermaid-js/mermaid-cli
bash scripts/export_mermaid.sh
```

Outputs are written to `docs/diagrams/<doc-name>/diagram-XX.png|svg`.


---
## Acknowledgments

SDOM builds upon the excellent foundation provided by the [Simple DirectMedia Layer (SDL)](https://libsdl.org) project.  
All credit for SDL’s architecture, performance, and portability goes to its maintainers and contributors — especially **Sam Lantinga** and the [SDL team](https://github.com/libsdl-org).

Special thanks to **JeanHeyd “ThePhD” Meneide**, author of [Sol2](https://github.com/ThePhD/sol2) and the upcoming [Sol3](https://github.com/ThePhD/sol3) —  
whose work has made high-quality Lua integration in C++ both elegant and practical.

SDOM would not be possible without the open-source community’s contributions to  
**[SDL3](https://github.com/libsdl-org/SDL)**, **[Sol2](https://github.com/ThePhD/sol2)**, and the **[Lua](https://www.lua.org/)** ecosystem.

---
## License
ZLIB License. 

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.
 
    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:
 
    1. The origin of this software must not be misrepresented; you must not
        claim that you wrote the original software. If you use this software
        in a product, an acknowledgment in the product documentation would be
        appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
        misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.


---
## Author
Jay Faries ([warte67](https://github.com/warte67))

🦆 "Performing a stable sort on the Anseriformes array by ascending position along the x-axis, ensuring deterministic alignment and zero rotational variance." (translation: "Getting my ducks in a row")
