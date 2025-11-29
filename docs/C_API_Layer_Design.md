# SDOM C API Layer Design

## Overview

This document outlines the design of the **SDOM C API Layer**, a procedural interface modeled after SDL.  
It provides a familiar, minimal C entry point into the C++ SDOM Core while maintaining type safety, encapsulation, and optional cleanup semantics.

---

## Goals

- 🧠 **Familiarity**: Resemble SDL’s procedural API for easy adoption.  
- 🧩 **Encapsulation**: Hide all internal C++ implementation details.  
- ⚙️ **Safety**: Allow optional cleanup via smart pointer semantics.  
- 🌍 **Interop**: Provide a stable ABI for C, Rust, and other foreign language bindings.  
- 🔄 **Consistency**: Mirror the Core’s functionality in a procedural form.

---

## Architecture

### Layer Structure

```
+------------------------------+
|   C API Layer (Procedural)   |
|   SDOM_Init, SDOM_Quit, etc. |
+------------------------------+
|   C++ Core Singleton Layer   |
|   Core, Window, Factory, etc.|
+------------------------------+
|   Internal Engine Subsystems |
|   EventManager, Renderer...  |
+------------------------------+
```

---

## Example Usage

### SDL Reference

```c
SDL_Init(...);
SDL_Window* window = SDL_CreateWindow("Title", 800, 600, SDL_WINDOW_RESIZABLE);
SDL_DestroyWindow(window);
SDL_Quit();
```

### SDOM Equivalent

```c
const uint64_t init_flags = 0; // reserved for future SDL-style options
SDOM_Init(init_flags);

CoreConfig init = {0};
init.title = "SDOM Window Title";
init.windowWidth = 800.0f;
init.windowHeight = 600.0f;
init.pixelWidth = 2.0f;
init.pixelHeight = 2.0f;
init.windowFlags = SDL_WINDOW_RESIZABLE;
init.pixelFormat = SDL_PIXELFORMAT_RGBA8888;
init.rendererLogicalPresentation = SDL_LOGICAL_PRESENTATION_LETTERBOX;

DisplayHandle window_handle = SDOM_CreateWindow(init);
SDOM_DestroyWindow(window_handle);  // optional
SDOM_Quit(); // optional
```

---

## C API Design

### Initialization and Shutdown

```cpp
extern "C" bool SDOM_Init(uint64_t init_flags) {
    (void)init_flags; // placeholder until flags are honored inside Core
    return Core::getInstance().onInit();
}

extern "C" void SDOM_Quit() {
    Core::getInstance().onQuit();
}
```

### Core Accessor

```cpp
extern "C" SDOM_Core* SDOM_GetCore() {
    return &Core::getInstance();
}
```

### Window Management

```cpp
extern "C" DisplayHandle SDOM_CreateWindow(CoreConfig init) {
    return Core::getInstance().createWindow(init);
}

extern "C" void SDOM_DestroyWindow(DisplayHandle handle) {
    Core::getInstance().destroyWindow(handle);
}
```

---

## Opaque Handles

To maintain ABI stability and prevent direct access to internal C++ classes:

```c
typedef struct SDOM_Core SDOM_Core;
typedef struct SDOM_DisplayHandle SDOM_DisplayHandle;
typedef struct SDOM_Event SDOM_Event;
```

These are simply forward declarations — C users only ever receive pointers or handles, never actual object types.

---

## Rust Interop

### Raw Bindings

```rust
extern "C" {
    fn SDOM_Init(init_flags: u64) -> bool;
    fn SDOM_Quit();
    fn SDOM_CreateWindow(init: CoreConfig) -> DisplayHandle;
    fn SDOM_DestroyWindow(handle: DisplayHandle);
}
```

### Safe Wrapper

```rust
pub struct SDOMWindow {
    handle: DisplayHandle,
}

impl SDOMWindow {
    pub fn new(cfg: CoreConfig) -> SDOMWindow {
        unsafe {
            let init_flags: u64 = 0;
            if !SDOM_Init(init_flags) {
                panic!("SDOM_Init failed");
            }
            let handle = SDOM_CreateWindow(cfg);
            SDOMWindow { handle }
        }
    }
}

impl Drop for SDOMWindow {
    fn drop(&mut self) {
        unsafe { SDOM_DestroyWindow(self.handle) };
        unsafe { SDOM_Quit() };
    }
}
```

Rust calls are `unsafe` (standard for FFI), but the abstraction layer ensures safe and automatic cleanup.

---

## Optional Smart Cleanup

Because Core and DisplayHandle are managed with internal reference counting, explicit calls to:

```c
SDOM_DestroyWindow()
SDOM_Quit()
```

are **optional**.  
They can be used for deterministic cleanup but are not required for safety.

---

## Function Prefixing

All exported symbols should use the **`SDOM_`** prefix for clarity and namespace safety.

Example API surface:

```c
SDOM_Init (uint64_t init_flags)
SDOM_Quit
SDOM_CreateWindow
SDOM_DestroyWindow
SDOM_LoadAsset
SDOM_RunLoop
SDOM_RegisterCallback
```

---

## Implementation Notes

| Layer | Responsibility |
|-------|----------------|
| **C API** | Entry point, opaque handles, procedural calls |
| **Core** | Singleton manager, lifecycle, interface routing |
| **Window** | SDL3 initialization, rendering context, texture ownership |
| **EventManager** | Event routing, input dispatch, global queue |
| **Factory** | DisplayObject and AssetObject creation |

---

## Benefits Summary

| Feature | Benefit |
|----------|----------|
| 🔁 Familiar SDL-style API | Easy transition for developers |
| 🧩 Fully encapsulated | Internals hidden behind opaque handles |
| 🧠 Singleton-backed | Global access through SDOM_GetCore |
| 🧱 ABI Stable | Safe for FFI across languages |
| ⚙️ Optional cleanup | Uses smart lifetime management |
| 🦀 Rust-ready | Simple unsafe → safe wrapper layer |

---

## Future Considerations

- `SDOM_Run()` — optional main loop runner for embedded integration.  
- `SDOM_RegisterCallback()` — hook Lua or user functions directly from C.  
- `SDOM_LoadModule()` — dynamically load subsystems or test modules.  
- `SDOM_GetFunctionByID()` — bridge with the Core Function Registry for unified reflection.

---

## Summary

The **SDOM C API**:
- Mirrors SDL’s interface for immediate familiarity.
- Forwards all logic to the C++ Core singleton.
- Provides safe, opaque handles for external use.
- Enables clean, low-friction bindings in C, Rust, Zig, and more.

---

---

## Optional Startup Scenarios

The C API provides flexibility in how applications initialize and register callbacks.  
Developers can either run fully manual loops or delegate control to SDOM through lifecycle callbacks.

### Example: Callback-Based Lifecycle

```c
#include <SDOM/SDOM.hpp>  // umbrella header

// Callback Prototypes:
bool fnOnInit();
void fnOnQuit();
void fnOnUpdate(float dFrame);
void fnOnEvent(SDOM_Event& evnt);
void fnOnRender();
bool fnOnUnitTest(int frame);  // optional
void fnOnWindowResize();       // optional

int main(int argc, char** argv) {
    const uint64_t init_flags = 0;
    SDOM_Init(init_flags);

    // Create window as before
    CoreConfig init = {0};
    init.title = "SDOM Window Title";
    init.windowWidth = 800.0f;
    init.windowHeight = 600.0f;
    init.pixelWidth = 2.0f;     // virtual pixels per physical pixel (final texture size 400x300)
    init.pixelHeight = 2.0f;    // virtual pixels per physical pixel (final texture size 400x300)
    init.windowFlags = SDL_WINDOW_RESIZABLE;
    init.pixelFormat = SDL_PIXELFORMAT_RGBA8888;
    init.rendererLogicalPresentation = SDL_LOGICAL_PRESENTATION_LETTERBOX;

    DisplayHandle window_handle = SDOM_CreateWindow(init);

    // Register Lifecycle Callbacks
    SDOM_RegisterOnInit(fnOnInit);
    SDOM_RegisterOnQuit(fnOnQuit);
    SDOM_RegisterOnUpdate(fnOnUpdate);
    SDOM_RegisterOnEvent(fnOnEvent);
    SDOM_RegisterOnRender(fnOnRender);
    SDOM_RegisterOnUnitTest(fnOnUnitTest);
    SDOM_RegisterOnWindowResize(fnOnWindowResize);

    // Optional: SDOM internal loop could be run here
    // SDOM_RunLoop();

    return 0;
}

// Callback Implementations
bool fnOnInit() { /* initialization logic */ return true; }
void fnOnQuit() { /* cleanup logic */ }
void fnOnUpdate(float dFrame) { /* per-frame update logic */ }
void fnOnEvent(SDOM_Event& evnt) { /* handle input events */ }
void fnOnRender() { /* draw frame */ }
bool fnOnUnitTest(int frame) { /* optional test hooks */ return true; }
void fnOnWindowResize() { /* handle window resize */ }
```

### Notes

- Callbacks mirror the **Core lifecycle** structure exactly:
  - `onInit`, `onQuit`, `onUpdate`, `onEvent`, `onRender`, `onUnitTest`, and `onWindowResize`.
- All registered callbacks are optional — if omitted, SDOM will safely skip them.
- The engine can optionally run its own loop via `SDOM_RunLoop()` or allow the host to drive updates manually.
- Future bindings (e.g., Lua, Python, Rust) can expose these callbacks natively for integration with user scripts.

---
