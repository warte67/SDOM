
# SDOM Unified C API & Lua Binding System  
### **Comprehensive Design Document — Full Architecture With Lua Headers & Stubs**

---

# 1. Overview

This document defines the **complete architecture** for SDOM’s:

- **Unified C API**
- **Lua Binding System**
- **Lua Header/Stub IntelliSense Generation**
- **SUPER/inheritance-aware binding reuse**
- **Per-object generated C++ files**
- **Per-object Lua headers**
- **Master lua/api_stubs.lua file**

All produced automatically by SDOM’s **BindGenerator**, driven by runtime reflection metadata collected in the `DataRegistry`.

The resulting system ensures:

- ✔ One universal handle type  
- ✔ Unified C API entry points  
- ✔ Per-object method tables (no collisions)  
- ✔ Automatic inheritance (SUPER chain reuse)  
- ✔ IDE-friendly Lua type stubs and enums  
- ✔ Runtime Lua bindings mapped to C API  
- ✔ Deterministic and maintainable generated code  

---

# 2. Goals

### Primary Goals
- **Unified C ABI**
- **Per-type method tables**
- **Per-type generated files** for both C & Lua
- **Reflection-driven automation**
- **Zero manual binding code**
- **Safe error-handling for unsupported functions**
- **Efficient dispatch via handle.method table pointers**
- **IDE-friendly Lua autocompletion**

### Secondary Goals
- Expandable to other languages (Rust, Zig, Python)
- Deterministic output  
- Inheritance-respecting binding logic  
- Codegen steps integrated cleanly into CMake  

---

# 3. BindGenerator Metadata Pipeline

The BindGenerator walks the SDOM `DataRegistry` and collects:

- All object types
- Their C++ type identifiers
- SUPER / inheritance relationships
- All exported functions
- All properties
- All enums
- Documentation strings
- API export names
- Lua-friendly names
- C API signatures
- Any overrides

It constructs an **inheritance model** and a **global function histogram** of all function identifiers.

---

# 4. Unified SDOM Handle

Generated:

```c
typedef struct SDOM_Handle {
    void*       obj;        // underlying C++ object
    uint32_t    type;       // type ID, used to index method tables
    const void* methods;    // pointer to per-type method table
    const char* name;       // optional debug name
    const char* type_str;   // debug-friendly type string
} SDOM_Handle;
```

Handles are passed to all C API functions as the first parameter.

---

# 5. Per-Type Function Tables

For each object type `T`, the generator produces:

**Header:**

```c
// SDOM_CAPI_T.hpp
typedef struct SDOM_TMethods {
    bool (*SetX)(void*, int);
    bool (*SetY)(void*, int);
    bool (*SetCaption)(void*, const char*);
    bool (*Click)(void*);
    // Inherited & local functions collected automatically
} SDOM_TMethods;

extern const SDOM_TMethods g_SDOM_TMethods;
```

**Source:**

```cpp
// SDOM_CAPI_T.cpp
#include "SDOM_CAPI_T.hpp"
#include <SDOM/SDOM_T.hpp>

static bool T_SetCaption(void* obj, const char* caption) {
    auto* t = static_cast<SDOM::T*>(obj);
    if (!t) return false;
    t->setCaption(caption);
    return true;
}

const SDOM_TMethods g_SDOM_TMethods = {
    &IDisplayObject_SetX,
    &IDisplayObject_SetY,
    &T_SetCaption,
    &T_Click
};
```

SUPER chain functions are reused automatically.

---

# 6. Global Method Table Index

A central table ties type IDs → per-type method tables:

```cpp
const void* g_SDOM_MethodTables[SDOM_TYPE_COUNT] = {
    &g_SDOM_ButtonMethods,
    &g_SDOM_LabelMethods,
    &g_SDOM_SpriteMethods,
    ...
};
```

---

# 7. Unified C API Surface

For every function identifier in the histogram, the generator outputs:

**Header:**

```c
bool SDOM_SetCaption(SDOM_Handle h, const char* caption);
bool SDOM_SetPosition(SDOM_Handle h, int x, int y);
...
```

**Source:**

```cpp
extern "C" bool SDOM_SetCaption(SDOM_Handle h, const char* caption) {
    clearError();

    auto* table = (const SDOM_TMethods*) g_SDOM_MethodTables[h.type];

    if (!table || !table->SetCaption) {
        SDOM_SetError("Function 'SetCaption' is undefined for '%s'", h.type_str);
        return false;
    }
    return table->SetCaption(h.obj, caption);
}
```

Missing functions return `false` and update the error buffer.

---

# 8. File Generation Layout

## 8.1 Core CAPI Files (Generated Once)

```
SDOM_CAPI_Core.hpp
SDOM_CAPI_Core.cpp
SDOM_CAPI_Error.hpp
SDOM_CAPI_Error.cpp
SDOM_CAPI_Types.hpp
SDOM_CAPI_TypeTables.cpp
SDOM_ObjectTypeIds.hpp
```

These contain:

- `extern "C"` entry points
- Method dispatchers
- Error buffer logic
- Handle utilities
- Type ID definitions

---

## 8.2 Per-Object C API Files (Generated Per Type)

For each object type `T`:

```
SDOM_CAPI_T.hpp     // struct + extern method table
SDOM_CAPI_T.cpp     // implementation + method table definition
```

---

# 9. Lua Binding Output (Complete System)

The Lua binding system consists of **three layers**:

---

## 9.1 **Lua API Headers (`lua/SDOM/T.lua`)**

One file per object type.

Inside each file:

- Class definition (with `---@class`)
- SUPER inheritance (using `: SDOM.SuperType`)
- Method stubs (with full signatures)
- Enum tables
- Structs
- Descriptions

### Example (auto-generated):

```lua
---@class SDOM.Button : SDOM.DisplayObject
---@field caption string
local Button = {}

---@param text string
function Button:SetCaption(text) end

function Button:Click() end

return Button
```

These files provide **IntelliSense and documentation**, not runtime behavior.

---

## 9.2 **Master IntelliSense Stub (`lua/api_stubs.lua`)**

Aggregated file containing:

- All object types
- Namespace layout
- Inherited methods
- Enums
- All parameter annotations

Generated every build.

Example:

```lua
---@class SDOM.DisplayObject
SDOM.DisplayObject = {}

---@class SDOM.Button : SDOM.DisplayObject
SDOM.Button = {}

function SDOM.Button:SetCaption(text) end
function SDOM.Button:Click() end
```

This is the file Lua IDEs use for autocomplete.

---

## 9.3 Runtime Bindings (C++ → Lua)

Files:

```
SDOM_LUA_Core.cpp
SDOM_LUA_T.cpp   (one per type)
```

BindGenerator emits C++ code using sol2, e.g.:

```cpp
lua.new_usertype<SDOM_Handle>("Button",
    "SetCaption", [](SDOM_Handle h, const std::string& s){
        if (!SDOM_SetCaption(h, s.c_str()))
            throw std::runtime_error(SDOM_GetError());
    },
    "Click", [](SDOM_Handle h){
        if (!SDOM_Click(h))
            throw std::runtime_error(SDOM_GetError());
    }
);
```

Inheritance is achieved by chaining metatables.

---

# 10. SUPER Chain Reuse

The generator uses SUPER chains to:

- Collect inherited methods
- Reuse inherited C++ implementations
- Reuse inherited Lua bindings
- Build object type trees
- Ensure each derived type table includes inherited entries

This prevents duplicate code and ensures consistency.

---

# 11. BindGenerator Workflow (Final)

### Step 1 — Walk registry  
Collect types, supers, functions, properties, enums.

### Step 2 — Build global function histogram  
Sorted list of all unique function names.

### Step 3 — Emit per-type C API method tables  
Resolve inherited + overridden functions.

### Step 4 — Emit per-type C++ implementations  
Only for functions owned by the object type.

### Step 5 — Emit unified C API dispatch surface  
One dispatcher per function name.

### Step 6 — Emit Lua API headers (`lua/SDOM/T.lua`)  
Stubs for IDE, one per object type.

### Step 7 — Emit master IntelliSense stub `api_stubs.lua`  
Aggregated view of entire API.

### Step 8 — Emit runtime Lua bindings (`SDOM_LUA_T.cpp`)  
Actual callable Lua functions.

### Step 9 — Emit diagnostics files (optional)  
Histogram, type hierarchy, method signature maps.

---

# 12. Error Handling

All SDOM C API functions return:

```c
false + SDOM_SetError("...description...");
```

Lua bindings convert these to exceptions.

This guarantees:

- No crashes on invalid calls
- Clean error messages
- Predictable failure mode

---

# 13. Directory Layout Example

```
generated/
    SDOM_CAPI_Core.hpp
    SDOM_CAPI_Core.cpp
    SDOM_CAPI_Error.cpp
    SDOM_CAPI_Types.hpp
    SDOM_CAPI_Button.hpp
    SDOM_CAPI_Button.cpp
    SDOM_CAPI_Label.hpp
    SDOM_CAPI_Label.cpp
    ...
    SDOM_LUA_Core.cpp
    SDOM_LUA_Button.cpp
    SDOM_LUA_Label.cpp
    ...
    lua/SDOM/Button.lua
    lua/SDOM/Label.lua
    lua/api_stubs.lua
```

---

# 14. Conclusion


## Reviewer Notes — Suggestions, Risks, and Next Steps

Below are review points, concrete suggestions, and recommended next steps to make the design more robust and implementation-ready. These complement the existing architecture and help guide the generator and C API implementation.

1) Handle layout and ABI stability
- Keep `SDOM_Handle` compact and fixed-size for ABI stability. If fields are added later, prefer a versioned layout (e.g., `SDOM_Handle_v2`) rather than changing the original struct in-place.
- Document alignment/packing rules and guarantee the convention for the invalid handle value (e.g., `obj == nullptr || object_id == 0` means invalid).

2) Type IDs and method-table indexing
- Generate a deterministic `ObjectTypeId` enum (uint32_t) early in the generation pipeline and emit a header such as `SDOM_ObjectTypeIds.hpp` so all generated sources can use stable indices.
- Validate type id at runtime before indexing into `g_SDOM_MethodTables` to avoid out-of-bounds access.

3) Per-type method tables & inheritance
- Flatten the SUPER chain at generation time so each per-type method table contains inherited entries filled from parents. This avoids repeated lookup logic at runtime and simplifies dispatch.
- When overrides occur, ensure the child table replaces the parent entry for that slot.

4) Unified dispatchers and fast paths
- The generator's plan for top-level `SDOM_X` dispatchers is correct and simple. These should check `h.type`, pick the table, validate the function pointer, and call it.
- Consider emitting optional inline fast-path helpers in headers for hot-call sites where the caller knows the type (e.g., `inline bool SDOM_SetCaption_fast(SDOM_Handle h, ...)`).

5) Error handling
- Continue to use `SDOM_SetError` / `SDOM_GetError`. Prefer a thread-local last-error buffer to avoid cross-thread races.
- All C API entry points should return `bool` (or an error sentinel) and set a descriptive error string on failure.

6) Handle lifetime and validity
- Prefer a hybrid handle that stores both a stable `object_id` and a cached `obj` pointer. Dispatchers should try the cached pointer first; if stale/NULL, resolve via `Factory` using `object_id` and update the cache.
- This balances performance (fast direct pointer use) and safety (stable ID resolution if pointer expired).

7) Ownership & memory rules
- Document ownership for `const char*` fields exposed in `SDOM_Handle` (e.g., `name`, `type_str`). Prefer generator-owned string storage lifetime guarantees or require callers to copy strings if they need them beyond the call.

8) Generator architecture & deterministic output
- Keep the BindGenerator single-pass and deterministic: (1) collect registry, (2) topologically sort types, (3) build method histogram, (4) produce per-type tables, (5) emit global dispatchers.
- Emit a generator-version header so consumers can detect changes in generated ABI.

9) Lua integration
- Generate `sol2` bindings that call the unified C API dispatchers. Convert C API `false` + `SDOM_GetError()` into Lua exceptions for clear failure semantics.

10) Incremental rollout / migration plan
- Start with a minimal subset: emit `SDOM_Handle`, `ObjectTypeIds`, method-table format, and a small set of dispatchers. Keep legacy bindings during migration and run tests exercising both code paths.

11) Performance & thread model
- Table lookup is O(1). For micro-optimizations, consider caching the method-table pointer inside `SDOM_Handle` at creation time (update atomically on type-reload if supported).
- Clarify thread-safety expectations for `Factory` and code that mutates method tables or the registry. If dynamic re-generation is supported, swap whole table arrays atomically.

12) Diagnostics & tooling
- Emit a diagnostics manifest (JSON) with type → methods mapping, function signatures, and inheritance graph for debugging and quick test generation.

13) Edge cases
- Disallow or disambiguate overloaded function names in the C ABI. Either mangle export names deterministically or reject ambiguous overloads during generation.

14) Mapping to current code
- Extend `FunctionInfo` as needed to include canonical function ids/signatures. The generator should build canonical function slots and then assemble per-type tables by filling slots from the SUPER chain.

15) Recommended tests
- Unit tests for: valid handle dispatch, invalid handle failure (with readable error), inheritance fallback (child inherits parent method), and Lua integration tests for error propagation and binding correctness.

If you want, I can now:
- Produce a concrete generator patch sketch that implements per-type table emission and the unified dispatcher codegen, or
- Draft the exact `SDOM_Handle` C struct layout and a small set of example generated headers/sources (method table + dispatcher) as a working prototype.

Which of those would you like me to do next?

This document defines the **complete**, **reflection-driven**,  
**inheritance-aware**, and **fully automated binding system** for SDOM.

It produces:

- Unified C API  
- Runtime Lua bindings  
- Lua development headers  
- IntelliSense stubs  
- Full SUPER inheritance support  
- Fast dispatch  
- Deterministic output  

This architecture ensures SDOM remains scalable, maintainable, and pleasant for end users across multiple languages.

---

*End of Document*
