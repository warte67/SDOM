
# SDOM Unified C API & Lua Binding System  
### **Comprehensive Design Document — Full Architecture (With Llama Review Integrated)**  
*Automatically Generated — November 23, 2025*

---

## 1. Overview

This document defines the **complete architecture** for SDOM’s binding layer:

- Unified C API
- Lua Binding System
- Opaque runtime handles
- Per-type method table dispatch
- Reflection-driven BindGenerator
- Object type enumeration and type IDs
- ABI stability and thread-safe error handling
- SUPER / inheritance flattening
- Diagnostics and deterministic code generation

The system behaves like a **reverse compiler**:

- SDOM’s runtime `DataRegistry` is treated as the “binary” or IR.
- The BindGenerator analyzes it (types, functions, supers, metadata).
- It then **emits source**: C API `.hpp/.cpp`, Lua runtime bindings, and Lua “header” stubs for IDE IntelliSense.
- Those generated sources are compiled into the **SDOM API library**.

The goal is to make SDOM’s internal C++ object model accessible from C, Lua, and future languages without exposing vtables or compromising ABI stability.

---

## 2. High-Level Goals

### 2.1 Primary Goals

- **One universal handle type** (`SDOM_Handle`) used by all C APIs and exposed to Lua.
- **Per-type method tables** that encode polymorphic behavior without exposing C++ virtual functions.
- **Unified C API surface** of functions like `bool SDOM_SetCaption(SDOM_Handle h, const char* text);`
- **Hybrid safety model**: handles carry both an object pointer and a factory-stable object ID.
- **Deterministic type IDs** (`SDOM_ObjectType_*`) and stable method table indexing.
- **Fully automated binding generation** for both C and Lua, with no hand-written glue.
- **Thread-safe error handling** (`SDOM_SetError` / `SDOM_GetError` with thread-local storage).
- **Inheritance-aware flattening** of methods from SUPER chains into per-type method tables.

### 2.2 Secondary Goals

- Preserve human-readable **PascalCase** naming consistently across C++, C, and Lua.
- Provide Lua “headers” and `api_stubs.lua` for IDE IntelliSense.
- Emit diagnostics manifests for debugging and test generation.
- Make it easy to extend to additional languages (Rust, Zig, Python, etc.) using the same pipeline.

---

## 3. Binding Architecture: “Reverse Compiler”

Traditional compiler pipeline:

> **Source → Parse → IR → Optimize → Codegen → Binary**

SDOM’s binding pipeline:

> **Runtime registry → Analyze → IR (types & functions) → Codegen → C & Lua source → API library**

- The **DataRegistry** is the IR: it tracks `TypeInfo`, `FunctionInfo`, `PropertyInfo`, enums, documentation, categories, and export metadata.
- The **BindGenerator** operates like a compiler backend:
  - walks the registry
  - resolves SUPER chains
  - builds a global function histogram
  - constructs per-type method tables
  - emits C and Lua code
- The generated C++ and Lua sources are compiled or loaded as part of the SDOM API library and Lua runtime.

This model scales with SDOM’s object system and provides a single source of truth: the runtime reflection.

---

## 4. Object Type Enumeration System

Every SDOM object type is assigned:

- A **unique, stable type ID** (a uint32_t).
- A canonical **C enum identifier**: `SDOM_ObjectType_<PascalCaseName>`.
- A mapping between:
  - C++ class name
  - enum identifier string
  - numeric ID
  - method table index
  - Lua-visible name

### 4.1 Naming Rules

- SDOM trusts the developer (you) to use **PascalCase** for object type names:
  - `Button`
  - `Label`
  - `SpriteSheet`
  - `Stage`
  - `Frame`
  - `DisplayObject`
- No automatic normalization is performed beyond what the C++ code already uses.
- Enum names are formed by simple prefixing:
  - `Button` → `SDOM_ObjectType_Button`
  - `Label` → `SDOM_ObjectType_Label`
  - `SpriteSheet` → `SDOM_ObjectType_SpriteSheet`

### 4.2 Factory Type Registration

Factory maintains the master list of object types:

```cpp
struct ObjectTypeInfo {
    uint32_t    id;        // numeric type ID
    std::string cppName;   // "Button"
    std::string enumName;  // "SDOM_ObjectType_Button"
    std::string fileStem;  // "Button"
    std::string exportName;// e.g. "SDOM_Button"
};

std::vector<ObjectTypeInfo> objectTypes;
std::unordered_map<std::string, uint32_t> objectTypeNameToId;
```

Registration (simplified):

```cpp
uint32_t Factory::registerObjectType(const std::string& cppName) {
    // ensure unique name (Factory already enforces this)
    if (objectTypeNameToId.contains(cppName)) {
        throw std::runtime_error("Duplicate SDOM object type: " + cppName);
    }

    uint32_t id = static_cast<uint32_t>(objectTypes.size());
    std::string enumName = "SDOM_ObjectType_" + cppName;

    ObjectTypeInfo info{ id, cppName, enumName, cppName, "SDOM_" + cppName };
    objectTypes.push_back(info);
    objectTypeNameToId[cppName] = id;

    return id;
}
```

BindGenerator reads this table to emit:

- `SDOM_ObjectTypeIds.hpp` (C++ enum/constexpr IDs)
- C enum definitions for the C API
- Lua enum tables (`SDOM.ObjectType = { Button = 0, Label = 1, ... }`)

---

## 5. SDOM_Handle: ABI-Stable Opaque Handle

### 5.1 Final Layout

```c
typedef struct SDOM_Handle {
    void*       obj;        // fast path: direct pointer to C++ instance (may be null or stale)
    uint64_t    object_id;  // factory-stable ID for safe resolution
    uint32_t    type;       // SDOM_ObjectType_* ID
    const char* name;       // optional debug name; non-owning pointer
    const char* type_str;   // optional type string; non-owning pointer
} SDOM_Handle;
```

### 5.2 ABI Rules

- Field order and sizes **must not change** once published.
- If a future revision requires additional data, define `SDOM_Handle_v2` rather than modifying this struct.
- `type == 0` (or a sentinel) should represent “invalid handle” if desired.
- Alignment and packing must be standard C layout (no compiler-specific packing).

### 5.3 Lifetime and Validity

Handles are:

- **Opaque**: C callers cannot legally interpret `obj` beyond passing it back to SDOM.
- **Non-owning**: Factory owns the underlying C++ objects; handles should not free them.
- **Potentially stale**: `obj` may become invalid if the object is destroyed or reallocated.

The hybrid model:

- Use `obj` for speed when known valid.
- Use `object_id` and `type` to re-resolve/validate via Factory when needed.

---

## 6. Factory, Object IDs, and Handle Resolution

Factory is responsible for:

- Creating and destroying objects.
- Assigning unique `object_id` values.
- Maintaining maps:
  - `id → object*`
  - `object* → id` (optional for reverse lookup)
- Providing a resolution API:

```cpp
class Factory {
public:
    static Factory& instance();

    SDOM::DisplayObject* resolveDisplayObject(uint64_t object_id) const;
    SDOM::AssetObject*   resolveAssetObject(uint64_t object_id) const;
    // or a unified resolve that returns a base type

    bool isValidObjectId(uint64_t object_id) const;
};
```

### 6.1 Handle Retargeting

When a handle is redirected at runtime to point to a different object:

```cpp
void SDOM_Handle_SetTarget(SDOM_Handle* h, SDOM::DisplayObject* obj, uint64_t object_id, uint32_t type) {
    h->obj       = obj;
    h->object_id = object_id;
    h->type      = type;
    // name/type_str may be updated for debug, but are optional
}
```

On call, the dispatcher:

1. Validates `h.type` is within bounds.
2. Uses `h.type` to select the correct method table.
3. Uses `h.obj` if non-null and known valid.
4. Optionally re-resolves via Factory and `object_id` if pointer validation fails.

The exact pointer validation mechanism is implementation-dependent, but the hybrid ID+pointer design supports both safety and speed.

---

## 7. Per-Type Method Tables

Each object type has a C struct containing function pointers for all methods that type supports, including inherited methods.

Example for `Button`:

```c
typedef struct SDOM_ButtonMethods {
    bool (*SetX)(void*, int);
    bool (*SetY)(void*, int);
    bool (*SetPosition)(void*, int, int);
    bool (*SetVisible)(void*, bool);
    bool (*SetCaption)(void*, const char*);
    bool (*Click)(void*);
} SDOM_ButtonMethods;

extern const SDOM_ButtonMethods g_SDOM_ButtonMethods;
```

### 7.1 SUPER Chain Flattening

Given a SUPER chain:

```text
Button → IPanelObject → IDisplayObject → IDataObject
```

The generator:

1. Gathers all functions from `IDataObject`, `IDisplayObject`, `IPanelObject`, and `Button`.
2. Builds a **global function histogram** of function identifiers.
3. For `Button`, constructs the final `SDOM_ButtonMethods` struct:
   - Child overrides (Button) replace parent entries.
   - Missing functions are filled from the nearest parent.
   - Functions not present in the SUPER chain remain `NULL` (or point to a null-stub).

This flattening happens at generation time, not runtime.

---

## 8. Global Method Table Registry

BindGenerator emits a global table mapping type IDs to method tables:

```cpp
const void* g_SDOM_MethodTables[SDOM_OBJECT_TYPE_COUNT] = {
    &g_SDOM_ButtonMethods,      // SDOM_ObjectType_Button
    &g_SDOM_LabelMethods,       // SDOM_ObjectType_Label
    &g_SDOM_SpriteSheetMethods, // SDOM_ObjectType_SpriteSheet
    // ...
};
```

Dispatchers use:

```cpp
auto* anyTable = g_SDOM_MethodTables[h.type];
```

and cast to the correct struct based on the function being implemented.

---

## 9. Unified C API Dispatch Layer

For each function identifier in the histogram (e.g. `"SetCaption"`), the generator emits:

### 9.1 C API Signature

```c
bool SDOM_SetCaption(SDOM_Handle h, const char* caption);
```

### 9.2 Implementation Pattern

```cpp
extern "C" bool SDOM_SetCaption(SDOM_Handle h, const char* caption) {
    SDOM_ClearError();

    if (h.type >= SDOM_OBJECT_TYPE_COUNT) {
        SDOM_SetError("Invalid SDOM_Handle type id (%u)", h.type);
        return false;
    }

    const void* anyTable = g_SDOM_MethodTables[h.type];
    if (!anyTable) {
        SDOM_SetError("No method table registered for object type id (%u)", h.type);
        return false;
    }

    const SDOM_ButtonMethods* mt = static_cast<const SDOM_ButtonMethods*>(anyTable);
    if (!mt->SetCaption) {
        SDOM_SetError("Function 'SetCaption' is undefined for object type '%s'", h.type_str);
        return false;
    }

    // Fast path: use pointer if available
    if (h.obj) {
        return mt->SetCaption(h.obj, caption);
    }

    // Optional: safe path resolution via object_id
    SDOM::DisplayObject* resolved = Factory::instance().resolveDisplayObject(h.object_id);
    if (!resolved) {
        SDOM_SetError("SDOM_Handle refers to an invalid or expired object (id=%llu)", (unsigned long long)h.object_id);
        return false;
    }

    return mt->SetCaption(static_cast<void*>(resolved), caption);
}
```

The exact cast (`Button*`, `DisplayObject*`, etc.) depends on the method’s declared owner type in the registry.

---

## 10. Error Handling (Thread-Safe)

All C API calls:

- return `bool`
- on failure, call `SDOM_SetError("...")`
- success leaves the previous error intact or clears it

### 10.1 Thread-Local Error Buffer

Implementation skeleton:

```cpp
static thread_local std::string g_lastError;

extern "C" const char* SDOM_GetError(void) {
    return g_lastError.empty() ? nullptr : g_lastError.c_str();
}

extern "C" void SDOM_SetError(const char* fmt, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    g_lastError.assign(buffer);
}

extern "C" void SDOM_ClearError(void) {
    g_lastError.clear();
}
```

This avoids cross-thread contention and keeps error reporting simple and predictable.

Lua bindings convert failed C calls into exceptions with `SDOM_GetError()` as the message.

---

## 11. Lua Binding System

Lua bindings are generated in **three layers**:

### 11.1 Lua API Headers (`lua/SDOM/T.lua`)

These are pure Lua files generated **per object type** and intended for:

- IDE autocomplete
- type hints / annotations
- documentation

Example `lua/SDOM/Button.lua`:

```lua
---@class SDOM.Button : SDOM.DisplayObject
---@field caption string
local Button = {}

---@param text string
function Button:SetCaption(text) end

function Button:Click() end

return Button
```

These files are not responsible for actual runtime behavior—they are developer-facing stubs.

---

### 11.2 Master IntelliSense Stub (`lua/api_stubs.lua`)

This file aggregates **all** SDOM classes, enums, and methods:

```lua
---@class SDOM.DisplayObject
SDOM.DisplayObject = {}

---@class SDOM.Button : SDOM.DisplayObject
SDOM.Button = {}

function SDOM.Button:SetCaption(text) end
function SDOM.Button:Click() end

SDOM.ObjectType = {
    Button = 0,
    Label  = 1,
    -- etc.
}
```

It is regenerated on each binding generation pass and is meant for LuaLS/EmmyLua and IDEs.

---

### 11.3 Runtime Lua Bindings (`SDOM_LUA_T.cpp`)

These are C++ files generated **per type** to wire Lua to the C API.

Example for `Button`:

```cpp
// SDOM_LUA_Button.cpp
#include "SDOM_LUA_Core.hpp"
#include "SDOM_CAPI_Button.hpp"

void SDOM_Lua_Register_Button(sol::state& lua) {
    lua.new_usertype<SDOM_Handle>("Button",
        "SetCaption", [](SDOM_Handle h, const std::string& text) {
            if (!SDOM_SetCaption(h, text.c_str())) {
                const char* err = SDOM_GetError();
                throw std::runtime_error(err ? err : "Unknown SDOM error");
            }
        },
        "Click", [](SDOM_Handle h) {
            if (!SDOM_Click(h)) {
                const char* err = SDOM_GetError();
                throw std::runtime_error(err ? err : "Unknown SDOM error");
            }
        }
        // inherited methods (e.g., SetX, SetY) are bound via shared helpers
    );
}
```

A central `SDOM_LUA_Core.cpp` will call each per-type registration function to populate the Lua environment.

Lua usage remains simple:

```lua
local btn = stage:GetObject("button1")
btn:SetCaption("Click Me")
btn:Click()
```

Behind the scenes, this calls the C API via the generated sol2 wrappers.

---

## 12. BindGenerator Workflow (Final)

The BindGenerator’s end-to-end pipeline:

1. **Collect registry**  
   - Read `TypeInfo`, `FunctionInfo`, `PropertyInfo`, enums.
   - Read Factory’s `ObjectTypeInfo` (type IDs and names).

2. **Topologically sort types by inheritance**  
   - Ensure SUPER types are processed before derived types.

3. **Build global function histogram**  
   - Collect all exported function identifiers.
   - Assign each a canonical ordering.

4. **Generate per-type method tables**  
   - Flatten SUPER chains:
     - inherit parent methods
     - override as needed
   - Construct `SDOM_TMethods` structs per type.

5. **Emit type ID header(s)**  
   - `SDOM_ObjectTypeIds.hpp` with enums/constexpr IDs.
   - Corresponding C enum for the C API.
   - Lua enum table (`SDOM.ObjectType`).

6. **Emit global method table registry**  
   - `g_SDOM_MethodTables[SDOM_OBJECT_TYPE_COUNT]`.

7. **Emit unified C API dispatchers**  
   - One function per method: `SDOM_Foo(SDOM_Handle h, ...)`.
   - Includes error handling and optional pointer resolution.

8. **Emit per-type C API implementation files**  
   - `SDOM_CAPI_T.hpp / .cpp` for each object type.

9. **Emit Lua API headers and stubs**  
   - `lua/SDOM/T.lua` for each object type.
   - `lua/api_stubs.lua` as global view.

10. **Emit runtime Lua bindings**  
    - `SDOM_LUA_T.cpp` for each object type.
    - `SDOM_LUA_Core.cpp` as central registrar.

11. **Emit diagnostics manifests** (optional)  
    - JSON/flat files describing:
      - type → methods → signatures
      - inheritance
      - table layouts

12. **Integrate with CMake**  
    - Generator runs as a pre-build step.
    - Generated `.cpp` and `.hpp` compiled into SDOM API library.

---

## 13. Thread Model and Safety Considerations

- Method tables are **immutable** after generation.
- Error buffer is **thread-local**.
- Factory must be thread-safe (or guarded) for handle resolution.
- Handles are POD and can be safely copied, passed by value, or stored in containers.
- C API does not expose any mutable global state beyond the error buffer.

---

## 14. Testing Strategy

Key tests to validate the system:

1. **Valid handle call**  
   - Create object, obtain handle, call a method.
   - Verify both success and side effect.

2. **Invalid handle type**  
   - Set `h.type` to an out-of-range value.
   - Ensure C API returns `false` and `SDOM_GetError()` is meaningful.

3. **Expired object id**  
   - Destroy object, keep handle, attempt call.
   - Confirm failure and readable error message.

4. **Inheritance behavior**  
   - Child type uses parent methods when not overridden.
   - Child overrides actually change behavior.

5. **Lua error translation**  
   - Force C API error from Lua call.
   - Confirm Lua sees an exception with the same error text.

6. **Retargeting**  
   - Rebind handle to another object of same or different type.
   - Ensure dispatch tables and type IDs behave correctly.

7. **Multithreaded error buffers**  
   - Concurrent calls from different threads.
   - Each thread’s `SDOM_GetError()` is independent.

---

## 15. Summary

This document describes a **robust, reflection-driven binding system** for SDOM:

- Opaque handles with hybrid pointer + ID safety.
- Per-type method tables that flatten SUPER chains.
- A unified C API with simple `bool` + error semantics.
- A three-layer Lua system: headers, stubs, and runtime bindings.
- A deterministic BindGenerator pipeline that turns runtime metadata into source.
- ABI stability, thread safety, and testability designed in from the start.

The result is a binding architecture that scales with SDOM, is pleasant to consume from C and Lua, and can be extended to additional languages with minimal effort.

---

*End of Document*
