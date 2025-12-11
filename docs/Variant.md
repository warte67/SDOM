# What Is a Variant in SDOM, and Why It Matters

SDOM treats **data as the common language** across all of its boundaries — C++, C, scripting languages, tools, and the runtime editor. The **Variant** type is the mechanism that makes that possible.

A Variant is SDOM’s **universal value type**: a structured, self-describing container that can represent everything from simple numbers to nested UI state, event payloads, JSON configuration, and even user-defined dynamic data.

Understanding Variant is understanding how SDOM moves information through the engine.

---

## 1. Purpose and Philosophy (Executive Summary)

SDOM is built on the idea that *languages shouldn’t fight over how data should look.* Instead, SDOM offers one consistent representation that:
- C++ uses internally
- the C ABI exposes externally
- Lua, Python, Rust, Zig, D, C#, etc. can bind to
- tools and future editors can inspect and modify
- JSON can serialize and deserialize losslessly

Variant exists so that every layer of SDOM — engine, tools, scripts, and external languages — can reason about data in the same way.

- Variant is a tagged, structured value that stores primitives, arrays, objects, handles, events, and dynamic user types.
- Arrays/objects share internal nodes (`std::shared_ptr`), so copying is cheap; JSON conversion produces deep copies when needed.
- Errors are first-class values (`VariantType::Error`), not hidden return codes.
- The C ABI (`SDOM_Variant`) mirrors the same structure so C, Rust, Zig, etc., can inspect the same payloads.
- Reflection, binding generation, event routing, and future editors all depend on Variant for predictable behavior.

It is not a “kitchen sink” type, nor a replacement for typed C++ structures. It is a **unification layer**, a neutral meeting ground between strongly typed code and dynamic runtime data.

---

## 2. Why SDOM Needs a Variant

SDL3 provides primitives and event structs; SDOM layers higher-level concepts such as display trees, inspectors, and data-driven widgets on top. Those systems need:

- Runtime serialization (config, undo/redo, persistence)
- Cross-language data transport (C ABI, scripting)
- Uniform event payloads and diagnostics
- Dynamic user-defined data without recompiling

Instead of inventing a unique struct for every subsystem, Variant gives SDOM a single, predictable value model.

---

## 3. What Problems Does Variant Solve?

### 1. Exchanging Data Across Languages

- **Without Variant:** each language would need its own struct layout and marshaling rules, quickly leading to drift.
- **With Variant:** every language—C++, C ABI consumers, future Lua/Python/Rust bindings—shares the same data model. `SDOM_Variant` mirrors the C++ class, so bindings stay consistent.

### 2. Inspecting and Editing Data at Runtime

- **Without Variant:** tools would need bespoke reflection code for each type, making inspectors brittle.
- **With Variant:** path-based access (`window.frame.buttons[2].label`) lets debuggers and editors traverse complex state safely. `getPath`, `setPath`, `erasePath`, and `VariantView` provide read/write access without exposing raw pointers.

### 3. Serializing/Deserializing Arbitrary Data

- **Without Variant:** every system (config, undo/redo, networking) would invent its own serializer.
- **With Variant:** `toJson` / `fromJson` provide a stable round-trip for configuration files, snapshots, undo buffers, and future network payloads.

### 4. Representing Engine Concepts Dynamically

- **Without Variant:** display handles, asset handles, events, and user-defined types would each need bespoke wrappers.
- **With Variant:** handles, events, and dynamic `shared_ptr` payloads sit in the same container. Converters registered with the DataRegistry make them visible to tools and scripts automatically.

#### TL;DR Table

| Problem Area | Without Variant | With Variant |
| --- | --- | --- |
| Property reflection | Custom glue per type | Properties become Variant objects |
| Cross-language access | Each language invents its own ABI | `SDOM_Variant` mirrors C++ Variant |
| Deep config editing | Pointer math / bespoke parsers | `getPath`, `setPath`, `erasePath` |
| Event metadata | One struct per event | Arbitrary JSON-like payloads |
| Diagnostics | Errors disappear in logs | Errors become inspectable `VariantType::Error` |
| Dynamic engine concepts | Ad hoc wrappers | Handles/events/dynamic types stored uniformly |

---

## 4. Data Model and Behavior

### Supported Types

| VariantType | Stored payload |
| --- | --- |
| `Null` | empty state |
| `Bool`, `Int`, `Real`, `String` | scalar primitives |
| `Array` | `std::vector<std::shared_ptr<Variant>>` |
| `Object` | `std::unordered_map<std::string, std::shared_ptr<Variant>>` |
| `Dynamic` | `VariantStorage::DynamicValue` (`shared_ptr<void>` + metadata) |
| `DisplayHandle`, `AssetHandle` | engine handle wrappers |
| `Event` | non-owning pointer to a live `SDOM::Event` |
| `Error` | message indicating what went wrong |

### Ownership & Aliasing

- Arrays/objects **alias**: copying them copies the shared pointer graph. Mutating one copy mutates them all.
- Scalars, handles, and error values copy by value.
- Dynamic values own their `shared_ptr<void>`.
- Events are non-owning pointers to the dispatching event; do not cache them beyond the dispatch.
- JSON round-trips (`toJson` / `fromJson`) produce deep copies when isolation is required.

### Teaching Note

Array/Object containers use *shared nodes* rather than deep copies. This means:

- Cloning or passing a Variant by value is cheap because it shares its internal graph.
- Mutating any alias mutates all other references to that node (by design).
- JSON → Variant → JSON round-trips preserve structure and produce deep copies only when explicitly requested.

This intentional design supports tooling, reflection, and inspectors, which frequently need to traverse the same data from multiple viewpoints without copying entire scene graphs.

### Mutation & Paths

- `set(key, value)`/`push(value)` mutate objects/arrays in place.
- `getPath("foo.bar[3]")` reads nested data without mutations or side effects.
- `setPath` expects the path to exist unless `PathOptions` request intermediate creation; otherwise it returns `false`.
- Failures never produce undefined behavior—they simply return `false` or, on the C side, set `SDOM_GetError()`.

### JSON Semantics

- `Variant::toJson()` walks the graph and emits deterministic JSON (depth limits + NaN rules available via `JsonConversionOptions`).
- `Variant::fromJson()` builds a fresh Variant graph, independent from any original source.
- Handles serialize to diagnostic objects (`{ "name": "...", "type": "..." }`); dynamic values rely on registered converters.

### Dynamic Converters

The converter registry extends Variant with user-defined C++ types. Register once, and converters handle JSON round-trips automatically:

```cpp
struct TestPoint { int x = 0; int y = 0; };

Variant::ConverterEntry ce;
ce.toJson = [](const VariantStorage::DynamicValue& dv) {
    auto p = std::static_pointer_cast<TestPoint>(dv.ptr);
    return nlohmann::json{ { "x", p ? p->x : 0 }, { "y", p ? p->y : 0 } };
};
ce.fromVariant = [](const Variant& v) -> std::shared_ptr<void> {
    if (!v.isObject()) return nullptr;
    auto p = std::make_shared<TestPoint>();
    if (const Variant* vx = v.get("x")) p->x = static_cast<int>(vx->toInt64());
    if (const Variant* vy = v.get("y")) p->y = static_cast<int>(vy->toInt64());
    return p;
};

Variant::registerConverter<TestPoint>("TestPoint", std::move(ce));
```

---

## 5. Guarantees & Invariants

| Guarantee | Details |
| --- | --- |
| Valid state | Variants never become “invalid”; errors are values (`VariantType::Error`). |
| Deterministic type tags | `Variant::type()` always matches the stored payload. |
| Alias semantics | Arrays/objects alias by design; scalars and handles do not. |
| Thread safety | Individual Variants aren’t thread-safe for concurrent mutation, but converter registration/lookup is guarded by a mutex. |
| ABI parity | `SDOM_Variant` mirrors the C++ layout and semantics. |

---

## 6. How Variant Differs from Other Engines

| Engine | Equivalent | SDOM’s emphasis |
| --- | --- | --- |
| Godot | `Variant` | ABI stability + JSON parity over scene serialization |
| Qt | `QVariant` | Adds handles, dynamic converters, and deterministic path helpers |
| Unreal | `FProperty`/`UStruct` metadata | Runtime-first, STL-based, tooling-friendly |

---

## 7. Using Variant in C++

### 7.1 Building and Reading Data

```cpp
Variant config = Variant::makeObject();
config.set("title", Variant("Hello SDOM"));

Variant window = Variant::makeObject();
window.set("width", Variant(int64_t(1280)));
window.set("height", Variant(int64_t(720)));
config.set("window", window);

Variant palette = Variant::makeArray();
palette.push(Variant("red"));
palette.push(Variant("green"));
palette.push(Variant("blue"));
config.set("palette", palette);

if (const Variant* wnd = config.get("window")) {
    fmt::print("{}x{}\n",
        wnd->get("width")->toInt64(),
        wnd->get("height")->toInt64());
}
```

### 7.2 Path Helpers

```cpp
Variant root = Variant::makeObject();
root.set("player", Variant::makeObject());
root.get("player")->set("hp", Variant(int64_t(95)));

Variant value;
if (root.getPath("player.hp", value)) {
    fmt::print("HP = {}\n", value.toInt64());
}

// Mutating existing scaffolding
root.set("stats", Variant::makeObject());
root.get("stats")->set("mana", Variant(int64_t(40)));
root.setPath("stats.mana", Variant(int64_t(60)));
```

### 7.3 Variant in Event Payloads

```cpp
#include <SDOM/SDOM_Event.hpp>

Event evt;

Variant payload = Variant::makeObject();
payload.set("playerId", Variant(int64_t(7)));
Variant stats = Variant::makeObject();
stats.set("hp", Variant(int64_t(90)));
stats.set("mana", Variant(int64_t(30)));
payload.set("stats", stats);

evt.setPayloadValueVariant("player", payload);

Variant player = evt.getPayloadValueVariant("player");
if (player.isObject()) {
    const Variant* hp = player.get("stats")->get("hp");
    fmt::print("Event HP = {}\n", hp ? hp->toInt64() : 0);
}
```

### 7.4 Cloning / Snapshot Strategies

Because arrays and objects alias, clone a Variant explicitly when you need isolation:

```cpp
Variant original = /* ... */;

// Deep copy via JSON round trip
Variant clone = Variant::fromJson(original.toJson());

// Safe to mutate clone without affecting original
clone.set("title", Variant("Mutated"));
```

This JSON round-trip is the recommended “snapshot” technique until dedicated clone helpers land.

### 7.5 Debug Logging

```cpp
Variant payload = evt.getPayloadValueVariant("player");
fmt::print("Payload snapshot:\n{}\n", payload.toDebugString(2));
```

`toDebugString(depth)` renders structured data with a depth limit, making it ideal for logging or console output.

### 7.6 Common Failure Modes

```cpp
Variant config = Variant::makeObject();
config.set("title", Variant("Sample"));

Variant out;

// Missing key
if (!config.getPath("window.width", out)) {
    fmt::print("window.width is missing (getPath returned false)\n");
}

// Scalar traversal (treating a number as an object)
config.set("version", Variant(int64_t(5)));
if (!config.getPath("version.major", out)) {
    fmt::print("Cannot traverse into scalar 'version'\n");
}

// Type mismatch
config.set("fullscreen", Variant(true));
if (const Variant* fullscreen = config.get("fullscreen")) {
    fmt::print("fullscreen as bool = {}\n", fullscreen->toBool());
    fmt::print("fullscreen as number reports {}\n", fullscreen->toInt64());
}
```

In C, call `SDOM_GetError()` after a failed API call to read the last error message.

### 7.7 Read-Only Views

```cpp
void dumpKeys(const Variant& v) {
    Variant::VariantView view(v);
    if (!view.isObject()) return;
    for (size_t i = 0; i < view.size(); ++i) {
        if (const Variant* child = view.at(i)) {
            fmt::print("{}\n", child->toString());
        }
    }
}
```

`VariantView` avoids deep copies when iterating read-only data (registry entries, inspector panes, etc.).

### 7.8 Preview: Lua Usage (Future Optional Layer)

Lua bindings are being rebuilt on top of the JSON/Variant pipeline. Once they return, the generated support files will let Lua scripts emit the same data structures shown above. A future workflow will look like this:

```lua
local Variant = require("SDOM.Variant")      -- generated support module
local Scene   = require("SDOM.SceneTypes")   -- DataRegistry-derived tables

local payload = Variant.object({
    playerId = 7,
    stats = { hp = 90, mana = 30 },
})

SDOM.SetEventPayload("player", payload)  -- calls into the C ABI with SDOM_Variant
```

Behind the scenes, the Lua helpers convert tables to JSON/Variant and call the same C ABI used by C++ and other languages. Even though Lua is optional, the data it produces stays byte-for-byte compatible with the native Variant representation.

---

## 8. Using Variant via the C ABI

### 8.1 Inspecting Values

```c
SDOM_Variant value = SDOM_MakeInt(42);
if (SDOM_IsInt(&value)) {
    printf("value = %lld\n", (long long)SDOM_AsInt(&value));
}
```

### 8.2 Event Payloads from C

```c
SDOM_Variant payload = SDOM_MakeNull();
if (SDOM_GetEventPayloadValue(evt, "player", &payload)) {
    SDOM_Variant hp = SDOM_MakeNull();
    if (SDOM_Variant_GetPath(&payload, "stats.hp", &hp) && SDOM_IsInt(&hp)) {
        printf("HP = %lld\n", (long long)SDOM_AsInt(&hp));
    }
}
```

### 8.3 Pushing Values into Existing Trees

```c
SDOM_Variant payload = SDOM_MakeNull();
if (SDOM_GetEventPayloadValue(evt, "player", &payload)) {
    SDOM_Variant hp = SDOM_MakeInt(120);
    if (!SDOM_Variant_SetPath(&payload, "stats.hp", &hp)) {
        fprintf(stderr, "SetPath failed: %s\n", SDOM_GetError());
    } else {
        SDOM_SetEventPayloadValue(evt, "player", &payload);
    }
}
```

`SDOM_Variant_SetPath` mirrors the C++ `setPath` semantics: it mutates the Variant if the path exists and returns `false` otherwise (setting `SDOM_GetError()`).

### 8.4 Handles and IDs

```c
SDOM_Variant handle = SDOM_MakeDisplayHandle(&display_handle);
if (SDOM_Handle_IsValid(&handle)) {
    printf("Object ID = %llu\n",
           (unsigned long long)SDOM_Handle_ObjectId(&handle));
}
```

---

## 9. Limitations & Non-Goals

| Topic | Current state |
| --- | --- |
| C ABI builders | No dedicated object/array factories yet; callers rely on existing scaffolding. |
| String ownership | `SDOM_MakeCString` stores a pointer; copy before the buffer goes out of scope. |
| Lua bridge | The previous `toLua`/`fromLuaObject` helpers are being redesigned; current bindings convert via C++ glue. |
| Thread safety | Individual Variants are not safe for concurrent mutation; clone per thread. |
| Binary blobs | Variant is not intended for raw buffer storage; use asset systems instead. |
| Schema validation | Variant carries values only; higher layers (DataRegistry) enforce structure. |

These constraints keep Variant lean and predictable for both tools and runtime code.

---

## 10. How Variant Embodies “Unification Over Division”

Variant is the common language across SDOM:

- C++ uses it internally for configuration, events, and reflection.
- The C ABI exposes the same shape, letting other languages interact without bespoke structs.
- Inspectors, binding generators, and tests consume the same value graph.
- JSON round-trips guarantee that tooling and runtime signatures stay in sync.

Rather than favoring one language or paradigm, SDOM creates a neutral data layer where multiple approaches can coexist.

---

## 11. Future Work

Variant continues to evolve, especially around the C ABI and scripting bridges. For a living checklist, design notes, and migration status, see [`docs/variant_update.md`](variant_update.md).
