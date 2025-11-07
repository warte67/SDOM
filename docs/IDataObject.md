# SDOM::IDataObject

IDataObject is the foundational interface for all data‑driven objects in SDOM. It defines the minimal lifecycle, identity, and Lua‑binding utilities that higher‑level types build upon (e.g., IDisplayObject, IAssetObject, and concrete classes like Button, TristateButton, SpriteSheet, etc.).

## Purpose
- Provide a consistent base for objects that are configured from data (Lua tables) and manipulated at runtime.
- Enable dynamic, idempotent Lua bindings without global collisions.
- Offer a lightweight lifecycle for initialization, teardown, and unit testing.

## Inheritance & Typical Usage
- Base: `SDOM::IUnitTest` (for unit‑test hooks)
- Derived (examples):
  - `SDOM::IDisplayObject` → Button, TristateButton, Label, …
  - `SDOM::IAssetObject` → Texture, SpriteSheet, BitmapFont, …

```
IDataObject
 ├─ IDisplayObject
 │   ├─ Button, TristateButton, Label, …
 │
 └─ IAssetObject
     ├─ Texture, SpriteSheet, BitmapFont, …
```

## Lifecycle
- `virtual bool onInit()` — called when the object is initialized
- `virtual void onQuit()` — called before the object is destroyed
- `virtual bool onUnitTest(int frame)` — optional frame‑by‑frame unit test hook

Derived classes should implement these as needed. The base does not impose rendering or event responsibilities; those live in higher‑level interfaces (e.g., IDisplayObject).

## Identity
- `std::string getName() const`
- `void setName(const std::string&)`

Every IDataObject has a human‑readable name used in editors, debugging, and registry lookups.

## Lua Binding Utilities
IDataObject provides two core helpers to create safe, idempotent bindings:

### ensure_sol_table(lua, typeName)
Returns a per‑type table in the Lua global namespace, creating it when absent. This is useful for type‑local utilities and legacy compatibility.

```cpp
sol::table typeTbl = IDataObject::ensure_sol_table(lua, "TristateButton");
```

### register_usertype_with_table<T, BaseT>(lua, typeName)
Registers (or reuses) a proper Lua usertype for `T` with base class `BaseT`, and marks it as registered so subsequent calls are no‑ops. This records inheritance for sol2 and avoids clobbering an existing usertype.

```cpp
// TristateButton derives from IDisplayObject
auto ut = IDataObject::register_usertype_with_table<TristateButton, IDisplayObject>(lua, TristateButton::TypeName);
```

## Binding Policy (High‑Level)
For collision‑free, maintainable bindings:

- Bind by type only — never on `DisplayHandle`/`AssetHandle`.
- Use a canonical registry table: `SDOM_Bindings[typeName]`.
- Keep `DisplayHandle` minimal (only `isValid`, `get`, `getName`, `getType`). Resolution into type methods happens via `DisplayHandle.__index`.

See docs/lua_binding.md for the full policy and lookup rules.

## Recommended Binding Pattern

1) Ensure a usertype and obtain per‑type tables:
```cpp
// Inside T::_registerLuaBindings(typeName, lua)
auto [typeTbl, reg] = IDataObject::ensure_type_tables<T, Base>(lua, typeName);

// Helper to publish on both the legacy per-type table and canonical registry
auto bind_type = [&](const char* name, auto&& fn) {
    if (!typeTbl[name].valid()) typeTbl.set_function(name, fn);
    if (!reg[name].valid())     reg.set_function(name, fn);
};
```

2) Bind functions that operate on the resolved object pointer (via DisplayHandle):
```cpp
bind_type("getText", [](DisplayHandle& h) -> std::string {
    if (auto* obj = h.as<T>()) return obj->getText();
    return {};
});

bind_type("setText", [](DisplayHandle& h, const std::string& s) {
    if (auto* obj = h.as<T>()) obj->setText(s);
});
```

3) Bind color getters/setters using SDL helpers (return tables of integers):
```cpp
bind_type("getLabelColor", [lua](DisplayHandle& h) -> sol::table {
    SDL_Color c{255,255,255,255};
    if (auto* obj = h.as<T>()) c = obj->getLabelColor();
    return SDL_Utils::get_lua_color(lua, c); // { r,g,b,a } + numeric indices
});
```

## Color & Property Conventions
- Getter returns a Lua table `{ r, g, b, a }` with numeric indices `1..4` as well (all ints 0..255).
- Setter accepts either an `SDL_Color` userdata or a Lua table with keyed or indexed fields.
- Use `bind_color_property(...)` or equivalent helpers to keep behavior consistent.

## Serialization
- Constructors that accept `sol::table` are responsible for reading configuration (deserialization).
- Objects may produce Lua tables representing their state for tooling (serialization). The exact format is left to the derived type.

## Introspection & Commands
- IDataObject is the natural home for a dynamic property/command registry in editor/debug builds. SDOM’s concrete classes typically expose their public API through Lua bindings as shown above; a richer runtime registry can be layered on top if desired.

## Best Practices
- Always call `register_usertype_with_table<T, Base>()` at the start of `_registerLuaBindings` for any concrete type.
- Publish type methods in `SDOM_Bindings[type]` and (optionally) in `globals[type]` for legacy scripts.
- Never add type methods to `DisplayHandle` or `AssetHandle`.
- Keep getters pure and return simple Lua types (numbers, strings, tables); avoid exposing raw pointers.
- Prefer integer color tables over userdata to avoid signed/unsigned surprises.

## See Also
- docs/lua_binding.md — Binding architecture, lookup rules, and refactor plan.
- include/SDOM/SDOM_IDataObject.hpp — Source of the lifecycle and binding helpers.
- src/SDOM_DisplayHandle.cpp — `__index` forwarding into `SDOM_Bindings`.
