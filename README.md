<a name="top"></a>

# SDOM: Simple SDL Document Object Model API
[![License](https://img.shields.io/badge/license-ZLIB-blue.svg)](LICENSE)
[![Documentation](https://img.shields.io/badge/docs-Doxygen-blue)](https://warte67.github.io/SDOM/)
[![Wiki](https://img.shields.io/badge/github-wiki-181717?logo=github)](https://github.com/warte67/SDOM/wiki)
[![Status](https://img.shields.io/badge/status-pre--alpha-orange.svg)](#project-status)

<!-- BEGIN_VERSION_BLOCK -->
**SDOM 0.5.279 (early pre-alpha)**  
**Build Date:** 2025-12-11_14:55:52  
**Platform:** Linux-x86_64  
**Compiler:** g++ (GCC) 15.2.1 20251112
<!-- END_VERSION_BLOCK -->

- GitHub Wiki: https://github.com/warte67/SDOM/wiki

> ⚠️ **Development Status:** SDOM is a **very early pre-alpha prototype**.  
> It is not feature-complete, APIs are unstable, and design work is ongoing.  
> This repository is public for collaboration and early feedback.

---

### Screenshot of the Test Harness application:
![Test Harness](https://raw.githubusercontent.com/warte67/SDOM/refs/heads/master/examples/test/docs/diagrams/SDOM_v0.5.244.png)

## Overview

**SDOM** (Software Display Object Model) is a C++23 framework that layers a clean, DOM-like display object hierarchy on top of SDL3, giving UI and scene elements explicit structure, properties, and behavior. While built in C++, SDOM’s architecture is intentionally language-agnostic: every property, event, and object passes through a reflection-driven data model that any language can speak. C++, C, Rust, Python, Lua—and even future declarative front-ends or HTML-like UI languages—can interact with SDL3 through the same unified, predictable interface. **SDOM unifies SDL3 across ecosystems, letting languages cooperate instead of compete by sharing one structural model for interactive applications.**

The system is both **cross-platform** and **cross-language**. SDOM exposes a stable, pointer-free **C ABI** that allows languages such as Rust, D, Zig, Python, C#, Swift, and others to call into the engine directly. Higher-level integrations—Lua today, and Python and others in the future—build on this ABI to support scripting, rapid prototyping, and live editing when desired, while remaining optional for projects that prefer a pure C++ workflow.

A central design goal of SDOM is **data-driven interoperability**. A unified reflection system records types, properties, and commands at runtime. The build system uses this metadata to **automatically generate bindings** for external languages: C API headers, Lua interfaces, and (eventually) Rust/D/Python modules—without handwritten glue code. Supporting a new language becomes a matter of writing a small **binding-generator backend**.

SDOM’s runtime is **JSON-native** via a single universal value type, `Variant`. Configuration, properties, and event payloads all use the same representation, enabling live inspection and modification. Applications that do not require dynamic behavior can use SDOM entirely from C++ with no scripting dependency, while dynamic languages can layer on top for interactivity.

The framework is built to be **modular and extensible**. Optional components—such as Lua integration, a runtime editor, or plugin systems—can be enabled or omitted depending on project needs. SDOM’s core architecture scales naturally across a wide range of applications: **game engines**, **custom UI editors**, **level editors**, **visual tooling**, **data-driven simulation interfaces**, **rapid-iteration prototypes**, and **embedded or scripting-augmented applications**. Future systems such as inspectors, live UI editors, and metadata-driven tools will continue to build on the same reflection and data model already in place. **SDOM’s design allows diverse workflows and languages to collaborate through one unified structural foundation.**

**Note:** SDOM is currently in **early pre-alpha**. APIs may change as the system evolves.

---

## Features
- Simple, composable API for UI and document structures
- Modern C++23 codebase
- Minimal dependencies (SDL3, SDL3_image, SDL3_ttf)
- Designed for clarity, maintainability, and extensibility

For a deep dive into SDOM's data model, see [docs/Variant.md](docs/Variant.md).

---

# 🌐 SDOM Design Philosophy: Unification Over Division

SDOM’s architecture is built around a practical goal:

> **Keep data consistent and usable across languages, tools, and runtime environments.**

Traditional game/tool frameworks often fragment functionality across multiple APIs or languages.  
SDOM approaches the problem differently: it organizes the engine around a **shared runtime type system** rather than around any single host language. SDOM is not designed to replace any language’s philosophy—it simply creates a shared space where different philosophies can work together.

### ✔ A unified type system
All engine-defined, user-defined, and runtime-generated types are recorded in the **DataRegistry**, providing a single source of truth for structure and metadata. Any language that can call the C ABI can interact with these types in a consistent way. SDOM is not intended as an *abstraction layer*—it acts as a **unification layer**, keeping data coherent across all integrations.

### ✔ A unified data representation
`Variant` provides a structural, language-agnostic value type for:

- primitives  
- arrays and objects  
- engine handles  
- event payloads  
- custom or dynamically defined types  

This allows data to flow cleanly between C++, C, Lua, Rust, Python, and future integrations without requiring separate hand-maintained representations.

### ✔ A unified ABI
The C ABI provides a stable interface that higher-level bindings can build on.  
Generated bindings stay synchronized with the registry, reducing duplication and avoiding divergence between languages.

Supported (current or planned):

- C++  
- C  
- Rust  
- D  
- Zig  
- Lua  
- Python  
- WASM  
- others via binding generation

### ✔ A unified editor and runtime
Future tooling (live editor, inspectors, debuggers) will rely on the same underlying metadata.  
Configuration, scripted behavior, and runtime editing all use the same reflected type system, helping keep engine, tools, and content consistent.

---

## Resulting Properties

The unification approach gives SDOM:

- a single data model shared across languages  
- consistent behavior for serialization and reflection  
- reduced maintenance cost when evolving types  
- predictable cross-language semantics  
- support for runtime-defined types without engine changes  

This philosophy aims to make SDOM flexible without becoming complex, and dynamic without compromising determinism.

---

## ✨ The Result

SDOM becomes more than an engine API:

### **It is a shared language for data across the entire development ecosystem.**

- C++ can use dynamic, editor-defined structs  
- Lua and Python can manipulate typed engine objects safely  
- Rust and Zig can write high-performance modules without losing type information  
- The runtime and editor become peers, not competitors  
- All layers talk through one unified model

No duplication.  
No divergence.  
No "C++ types vs scripting types."  
Just **one type system**, available everywhere.

---

# 🏛 Guiding Principle

> **Unification over Division.  
> Data over Language.  
> SDOM over Boundaries.**

---

# Performance Highlights (December 11, 2025):

> **Designed for modern workflows—runs on 2013 laptops.**

### 🧪 Real-World Test Environment
- Hardware: ~12-year-old mid-tier PC (no GPU reliance beyond SDL3)
- Build: **Release**, standard compiler optimizations enabled
- Scenario: Full SDOM UI sample with multiple widgets & event routing

### 📈 Results (Sample Release Build)

These numbers are from a representative **optimized release build** of the sample UI scene (≈40–50 active DisplayObjects) on the author’s development machine. They are intended as an order-of-magnitude reference, not a formal benchmark.

| Metric               | Result                 | Meaning                                  |
|----------------------|:----------------------:|------------------------------------------|
| Unit Tests           | **95 / 95 Passed**     | Stable core API and behavior             |
| Total Frame Time     | **~183 µs**            | ~5,000+ FPS theoretical headroom         |
| Top Widget Update    | **0.6–0.8 µs**         | Small per-node CPU cost                  |
| Top Widget Render    | **1.1–1.8 µs**         | Lightweight draw overhead per widget     |
| Memory Leaks         | **0 bytes leaked**     | Valgrind-clean for long-running tools    |

> These measurements reflect an **optimized release configuration**  
> with standard compiler optimizations enabled.  
> **Even micro-widgets** (radio/check buttons, labels)  
> stay under **1 µs update** and **2 µs render** cost in this configuration.

> Note: Exact performance depends on hardware, compiler, and configuration;  
> these figures are illustrative rather than a published benchmark.

### 🧩 Optional-Feature Architecture - Why It Matters

| Feature | Status | Cost if Disabled |
|---|---|---|
| Live Editor | Planned | **0 overhead** |
| Lua Scripting | Optional | **0 overhead** |
| In-engine diagnostics | Planned | **0 overhead** |
| Physics / Networking | Planned | **0 overhead** |

SDOM ships **only** the systems a project needs—no “always-on” bloat.

### 🛡️ Runtime Safety
- Threaded load tests pass
- Snapshot-safe Variant usage
- Deterministic error messaging
- Long-run allocation safety verified

---

> **Conclusion:**  
> SDOM isn’t just flexible—it’s **fast**.  
> The foundation is strong enough to power the future live editor  
> *without sacrificing performance on modest hardware.*

### A Note on C++ and Rust

SDOM is not built around the idea that a project must “choose” between C++ or Rust.  Both languages bring valuable strengths, and both have communities focused on correctness, performance, and long-term maintainability. Rather than treating them as opposing approaches, SDOM treats them as **peers** that can collaborate effectively.

- C++ offers mature tooling, predictable performance, and direct access to SDL3 and system APIs.
- Rust offers strong compile-time guarantees and a safety model that many developers rely on.

SDOM’s own architecture makes these differences complementary instead of conflicting:

- The framework enforces **clear ownership and lifetime rules** internally, removing the need for callers to manage safety-critical details.
- The **data-driven Variant system** eliminates the need for either language to interact directly with complex object graphs.
- The **C ABI** exposes SDOM in a stable, pointer-free form that Rust can use naturally without `unsafe` for ordinary API calls.
- C++ maintains its flexibility and expressiveness, while Rust receives a predictable, borrow-checker-friendly boundary.

The result is not “C++ vs Rust,” but **C++ and Rust working comfortably together**.

SDOM aims to be a *unifying layer*, not an abstraction layer—a way for different languages and ecosystems to share the same data model, the same type system, and the same runtime without friction or philosophical tension.

---

## Project Status

SDOM is currently in **early pre-alpha**.  
APIs are stabilizing, the Variant system is undergoing major ABI refinement,  
and the C ABI + language bindings infrastructure is actively evolving.

What *is* stable:
- Core rendering pipeline  
- DisplayObject tree model  
- Event routing  
- JSON-driven configuration  
- Variant core semantics (C++ side)  
- Unit test coverage (95/95 passing)

What *is still in flux*:
- C ABI Variant ownership/creation model  
- Lua/Python bindings  
- Runtime type editor & metadata tools  
- Autogenerated API codepaths  
- Documentation restructuring

This section will expand over time as subsystems lock into place.

[Back to top ↑](#top)


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
- `src/` - Core SDOM source code
- `include/SDOM/` - Public headers
- `examples/` - Example applications
- `docs/` - Documentation

---
# Lua Integration and Requirements

SDOM is designed to be **data-driven** and includes built-in support for scripting and configuration through **Lua (via Sol2)**.

### Why Lua?
Lua enables dynamic creation, modification, and serialization of DOM objects at runtime—letting you define UI layouts, animation sequences, and event behavior without recompiling. Lua scripts can access most of SDOM’s API, and future releases will expand this integration even further.

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
Follow the official SDL3 build instructions from the [SDL wiki](https://github.com/libsdl-org/SDL/wiki/Installation) or use the release artifacts from the [SDL GitHub repository](https://github.com/libsdl-org/SDL/releases) (the legacy `libsdl.org/download-2.0.php` page hosts SDL2, not SDL3).
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
Clone the repositories (run once per library):
```
git clone https://github.com/libsdl-org/SDL.git
git clone https://github.com/libsdl-org/SDL_mixer.git
git clone https://github.com/libsdl-org/SDL_image.git
git clone https://github.com/libsdl-org/SDL_ttf.git
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
include_directories(/usr/include/lua5.4)
find_library(LUA_LIB lua5.4)
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
All credit for SDL’s architecture, performance, and portability goes to its maintainers and contributors—especially **Sam Lantinga** and the [SDL team](https://github.com/libsdl-org).

Special thanks to **JeanHeyd “ThePhD” Meneide**, author of [Sol2](https://github.com/ThePhD/sol2) and the upcoming [Sol3](https://github.com/ThePhD/sol3)—  
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
