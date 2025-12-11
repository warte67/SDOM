# Architecture Overview

SDOM is a lightweight C++ runtime and display framework built around a clear separation of concerns:
**display objects** (the scene graph) and **asset objects** (resources such as images, fonts, audio, etc.).
Application code interacts with both through **DisplayHandle** and **AssetHandle**, which provide stable,
name-based references without exposing raw pointers.

At the center of the system is **Core**, a singleton responsible for configuration, window and renderer setup, the main loop, and coordination of events. **Core** delegates rendering and updates to the root **Stage** display object and works with the **EventManager** to deliver input and custom events using capture → target → bubble propagation across the display tree. SDL3-specific utilities and abstractions are contained within Core to isolate platform details and keep user-level code simple and testable.

Object and asset lifetimes flow through the **Factory**. Each concrete type registers its creation functions once; the Factory then creates objects by type and name on request. The Factory maintains a name→instance registry and returns typed handles instead of raw pointers, which decouples reference usage from ownership. This makes it straightforward to reload assets, inspect scene state, support live editing, and build external tooling around stable symbolic names. SDOM integrates cleanly into CMake-based applications as a static library (**libSDOM.a**); the repository includes example programs and an automated test harness. External dependencies center on the SDL3 family (**video**, **image**, **ttf**, **mixer**). Configuration and data-driven object creation are currently handled through JSON; higher-level bindings (Lua, Python, etc.) will layer on top of this neutral format.

The result is a compact, composable framework with well-defined extension points and a predictable runtime model.

## Scripting & Configuration Philosophy

### JSON Today, Optional Scripting Tomorrow

Earlier versions of SDOM embedded Lua directly into the runtime. That coupling has now been removed so the engine can stabilize its **core data model (Variant)** and **ABI surface** before reintroducing scripting.

**JSON is now the authoritative configuration and data description format.**  
All display object graphs, assets, and initialization paths flow through JSON and `Variant`, ensuring deterministic behavior, reproducible tests, and language-neutral tooling.

Lua will return—but as a *client* of this unified model rather than a foundational dependency:

- **Current state:**  
  SDOM loads configuration exclusively through JSON or direct C++ construction. No scripting layer is required.

- **Near-term plan:**  
  Reintroduce Lua as an **optional module** that reads/writes `Variant` and emits JSON-compatible structures. Lua becomes a thin, ergonomic convenience layer rather than an engine dependency.

- **Long-term strategy:**  
  Apply the same pattern for Python, Rust, D, and other languages—each interacting with SDOM via the stable C ABI and Variant.

This ensures scripting remains flexible and optional while the engine keeps a deterministic, testable core.

### Future Language Bindings

Because SDOM is organized around handles, centralized lifetimes, a C ABI, and a universal Variant type, it naturally supports a broad set of external languages. Planned bindings sit cleanly *above* the JSON/Variant layer and do not complicate the engine core.

| Language | Status | Notes |
| --- | --- | --- |
| **C++** | ✅ Native | Primary API surface |
| **C ABI** | ✅ Stable | Foundation for external language bindings |
| **Lua** | 🔄 Planned return | Will wrap the Variant/JSON pipeline; fully optional |
| **Python** | 🗺 Planned | Likely through a thin C ABI module (pybind11 or similar) |
| **Rust** | 🗺 Planned | Via `bindgen`, `cxx`, or custom wrappers over the C ABI |
| **D** | 🗺 Planned | BindBC or direct C-layer bindings |
| **Others** | 🗺 Feasible | Any language able to call a C ABI can integrate |

**The guiding rule:**
*SDOM never chooses a “preferred language.” It provides a neutral, consistent data model that any language can adopt without rewriting engine assumptions.*

---

## Configuration Format (JSON)

SDOM currently treats JSON as the canonical description for display graphs, assets, and application state. The JSON pipeline offers:

- Human-readable configuration files with comments (via SDOM’s parser extensions)
- Deterministic serialization via `Variant::toJson()` / `Variant::fromJson()`
- Structured diffs for hot reload or live editing
- A neutral format usable by tooling, scripting, and C++ alike

Lua (and other languages) will eventually sit on top of this pipeline: scripts will emit or consume JSON so that the underlying engine always sees a consistent representation.

---

## Summary

- SDOM is **native-first**, deterministic, and real-time–friendly.
- JSON is the **canonical configuration format**.
- Scripting (Lua, Python, Rust, etc.) is **optional** and will integrate via JSON/Variant + the C ABI.
- UI graph traversal and rendering **always run in optimized C++**.

---

## High-Level Architecture Diagram

```mermaid
%%{init: { 'theme': 'neutral', 'flowchart': { 'curve': 'linear' } }}%%
flowchart TB
  classDef core fill:#eef7ff,stroke:#4a90e2,color:#1a3b5d
  classDef mod  fill:#f7fff0,stroke:#7bb661,color:#234d20
  classDef iface fill:#fff8e6,stroke:#f0ad4e,color:#5a4500
  classDef util fill:#f5f5f5,stroke:#777,color:#222
  classDef dep  fill:#ede9ff,stroke:#7c6fe6,color:#262262
  classDef bin  fill:#e8f9f9,stroke:#22a6b3,color:#125a63

  subgraph APP[Application & Build]
    EX[examples/test/prog]:::bin
    LIB[(libSDOM.a)]:::bin
    CMake[CMake Build System]:::util
    EX -->|links| LIB
    CMake --> LIB
    CMake --> EX
  end

  subgraph SDOM[SDOM Library]
    Core["Core\n(SDL window/renderer)"]:::core
    Factory["Factory"]:::mod
    EventMgr["EventManager"]:::mod
    Stage["Stage (root)"]:::mod
    IDO["IDisplayObject"]:::iface
    IAO["IAssetObject"]:::iface
    DisplayHandle["DisplayHandle"]:::util
    AssetHandle["AssetHandle"]:::util
    Core --> Factory
    Core --> EventMgr
    Core --> Stage
    Factory --> IDO
    Factory --> IAO
    Stage --> IDO
    IDO --> DisplayHandle
    IAO --> AssetHandle
    EventMgr --> Stage
  end

  subgraph EXT[External Dependencies]
    SDL3[SDL3]:::dep
    SDLimg[SDL3_image]:::dep
    SDLttf[SDL3_ttf]:::dep
    SDLmix[SDL3_mixer]:::dep
    JSON[JSON Parser (nlohmann/json)]:::dep
  end

  Core --> SDL3
  Core --> SDLimg
  Core --> SDLttf
  Core --> SDLmix
  SDOM --> JSON
