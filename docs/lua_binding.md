# SDOM Lua Binding Architecture

This document describes the target architecture for SDOM’s Lua bindings. The aim is to make bindings simple, collision‑free, and easy to extend.

## Goals
- Bind by type only — never on DisplayHandle/AssetHandle.
- One canonical registry per type (no globals sprinkled randomly).
- Idempotent registration with clear base/derived relationships.
- Predictable lookup order with zero cross‑type collisions.
- Easy to read and extend.

## Binding Model

### Canonical Registry
- `SDOM_Bindings[typeName]` holds all methods for that concrete type.
- Base classes (e.g., `IDisplayObject`, `IPanelObject`) contribute their APIs into the same `SDOM_Bindings[typeName]` via the SUPER chain. There is no separate, shared `IDisplayObject` table used at runtime.
- Optional: asset types use the same `SDOM_Bindings` root.

### Usertype Registration
Each concrete type’s `_registerLuaBindings(typeName, lua)` should:
- Call `register_usertype_with_table<T, Base>(lua, TypeName)` to register the usertype idempotently and record base classes.
- Ensure `SDOM_Bindings[typeName]` exists.
- Never write functions on `DisplayHandle` or `AssetHandle`.

Example (TristateButton):

```cpp
auto ut = IDataObject::register_usertype_with_table<TristateButton, IDisplayObject>(lua, TristateButton::TypeName);
sol::table typeTbl = IDataObject::ensure_sol_table(lua, TristateButton::TypeName);
sol::table bindingsRoot = lua["SDOM_Bindings"];
if (!bindingsRoot.valid()) { bindingsRoot = lua.create_table(); lua["SDOM_Bindings"] = bindingsRoot; }
sol::table triTbl = bindingsRoot.raw_get_or(TristateButton::TypeName, sol::lua_nil);
if (!triTbl.valid()) { triTbl = lua.create_table(); bindingsRoot[TristateButton::TypeName] = triTbl; }

auto bind_type = [&](const char* name, auto&& fn) {
  if (!typeTbl[name].valid()) typeTbl.set_function(name, fn);
  if (!triTbl[name].valid())  triTbl.set_function(name, fn);
};

bind_type("getText", [](DisplayHandle& h){ if (auto* tb = h.as<TristateButton>()) return tb->getText(); return std::string{}; });
bind_type("setText", [](DisplayHandle& h, const std::string& s){ if (auto* tb = h.as<TristateButton>()) tb->setText(s); });
```

### Lookup Resolution
`DisplayHandle.__index` resolves methods without binding onto the handle:
1) `SDOM_Bindings[self:getType()][k]`
2) `globals[self:getType()][k]` (legacy, optional)
3) Minimal `DisplayHandle` table (only `isValid/get/getName/getType`)

### Property & Color Conventions
- Use uniform helpers:
  - `SDL_Utils::get_lua_color` → table of ints `{ r,g,b,a }` (and numeric indices 1..4)
  - `SDL_Utils::get__lua_color` → parse Lua table to `SDL_Color`
- Getters that represent colors should always return a table to avoid userdata/signed‑byte surprises.
- Provide consistent `set_property` / `bind_color_property` wrappers that operate on raw pointers, not handles.

## Refactor Phases

### 1) Infrastructure
- Add helpers:
  - `ensure_type_table(lua, type)` → returns/creates `SDOM_Bindings[type]` (and may create `globals[type]` alias if desired).
  - `bind_if_absent(tbl, name, fn)`, `bind_props(tbl, name, getter, setter)`.
- Make `DisplayHandle.__index` resolve strictly via `SDOM_Bindings[type]` first (with clean fallbacks).

### 2) Base Class Binders
- Update `IDisplayObject::_registerLuaBindings(typeName, lua)` so it binds its generic helpers into `SDOM_Bindings[typeName]` (not onto `DisplayHandle`, and not into a shared `IDisplayObject` table).
- Each derived type calls `SUPER::_registerLuaBindings(typeName, lua)`, accumulating inherited APIs into its own `SDOM_Bindings[typeName]`.

### 3) Concrete Display Types
- Button, TristateButton, IconButton, ArrowButton, etc.:
  - Call `register_usertype_with_table<T, IDisplayObject>()`.
  - Call `SUPER::_registerLuaBindings(typeName, lua)`.
  - Bind type-specific functions only to `SDOM_Bindings[typeName]`.
  - Use the color helpers for table-of-ints.

### 4) Assets
- SpriteSheet, Texture, BitmapFont, TruetypeFont, TTFAsset:
  - Mirror the same pattern: usertype + `SDOM_Bindings[type]`.
  - `AssetHandle` stays minimal.

### 5) Compatibility Shims (Optional)
- For one release, keep global `[type]` aliases alongside `SDOM_Bindings[type]` so old scripts don’t break.
- Remove later once scripts are migrated.

### 6) Tests & Docs
- Small Lua tests for each type:
  - Confirm method presence and behavior via `h:getType()` + `SDOM_Bindings[type]`.
- Add/maintain docs:
  - No binding onto DisplayHandle/AssetHandle
  - `register_usertype_with_table<T, Base>()`
  - Use `SDOM_Bindings[type]`
  - Use color conversion helpers

## Immediate Triage (Implemented)
- TristateButton:
  - Binds only onto its type table + `SDOM_Bindings["TristateButton"]`.
  - `register_usertype_with_table<TristateButton, IDisplayObject>()` called.
  - `getText`/`setText` bound to the type table; `DisplayHandle` resolves via `__index`.
- DisplayHandle:
  - `__index` resolves methods from `SDOM_Bindings[Type]` first, then `globals[Type]`, then minimal handle surface.
- IDisplayObject binder:
  - Target behavior is to bind generic helpers into `SDOM_Bindings[typeName]` via the SUPER chain (not onto the handle, and not into a shared `IDisplayObject` table).

## Suggested Next Session
- Ensure `IDisplayObject::_registerLuaBindings(typeName, lua)` binds generic helpers into `SDOM_Bindings[typeName]` via the SUPER chain (no direct `IDisplayObject` table).
- Sweep Button/Arrow/Icon/Check/Radio to bind only into `SDOM_Bindings[typeName]`.
- Sweep asset types similarly.
- Remove legacy handle-table bindings and any `ensure_handle_table` usages.

## SUPER Chain Binding Flow

Every concrete display type accumulates bindings from its base classes into a single per‑type table using the SUPER chain. Each level binds into `SDOM_Bindings[typeName]` using the same `typeName` value for the concrete type.

Example flow for a concrete type `Group : IPanelObject : IDisplayObject`:

```cpp
// IDisplayObject::_registerLuaBindings(typeName, lua)
(void)IDataObject::register_usertype_with_table<IDisplayObject, IDataObject>(lua, typeName);
sol::table reg = ensure_type_table(lua, "SDOM_Bindings", typeName); // SDOM_Bindings[typeName]
bind(reg, "getBounds",   /* ... */);
bind(reg, "setBounds",   /* ... */);
bind(reg, "getColor",    /* ... */);
bind(reg, "setColor",    /* ... */);

// IPanelObject::_registerLuaBindings(typeName, lua)
SUPER::_registerLuaBindings(typeName, lua); // contributes IDisplayObject API into SDOM_Bindings[typeName]
(void)IDataObject::register_usertype_with_table<IPanelObject, IDisplayObject>(lua, typeName);
sol::table reg = ensure_type_table(lua, "SDOM_Bindings", typeName);
bind(reg, "setBorder",   /* ... */);
bind(reg, "hasBorder",   /* ... */);

// Group::_registerLuaBindings(typeName, lua)
SUPER::_registerLuaBindings(typeName, lua); // contributes IPanelObject + IDisplayObject API into SDOM_Bindings[typeName]
(void)IDataObject::register_usertype_with_table<Group, IPanelObject>(lua, typeName);
sol::table reg = ensure_type_table(lua, "SDOM_Bindings", typeName);
bind(reg, "groupSpecific", /* ... */);
```

Lookup at runtime from a DisplayHandle `h`:

```lua
-- resolves in this order without binding onto DisplayHandle
h:getType()                      -- e.g., "Group"
h:getBounds()                    -- SDOM_Bindings["Group"].getBounds
h:setBorder(true)                -- SDOM_Bindings["Group"].setBorder
h:groupSpecific()                -- SDOM_Bindings["Group"].groupSpecific
```

Notes
- All inherited APIs (IDisplayObject → IPanelObject) end up in the same `SDOM_Bindings[typeName]` table.
- Concrete types must pass their own `typeName` down the SUPER chain.
- DisplayHandle remains minimal; it never hosts type APIs.

## DisplayHandle Behavior

DisplayHandle is a lightweight proxy (name + type) that resolves the correct Lua method table at call time. It does not host concrete type APIs.

### What DisplayHandle Stores
- `name_`: the object’s registry name.
- `type_`: the concrete type string (e.g., "Button", "TristateButton").

The Factory returns handles pre‑stamped with both values, e.g. `DisplayHandle(name, obj->getType())`.

### Resolution Algorithm (Lua `__index`)
When you call `h:getText()` or `h:setBorder(true)`, DisplayHandle’s `__index` resolves the function as follows:

1) `SDOM_Bindings[self:getType()][k]` (canonical per‑type table)
2) `globals[self:getType()][k]` (legacy compatibility, optional)
3) Minimal `DisplayHandle` table (only `isValid`, `get`, `getName`, `getType`)

No functions are bound onto DisplayHandle for concrete types. All type APIs live under `SDOM_Bindings[type]` and are accumulated through the SUPER chain.

### Call Wrapping
The bound function is invoked with the `DisplayHandle` (or as needed, with the resolved raw pointer) so that C++ lambdas can get the underlying object and perform the action safely:

```cpp
bind_type("setText", [](DisplayHandle& h, const std::string& s) {
  if (auto* obj = h.as<T>()) obj->setText(s);
});
```

This keeps the handle surface minimal while enabling type‑specific behavior through the canonical per‑type bindings.
