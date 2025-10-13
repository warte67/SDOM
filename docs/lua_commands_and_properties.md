# Lua commands and properties exposed by SDOM (C++ → Lua binding inventory)

This document lists the Lua API surface exported from the C++ SDOM core. It maps
Lua-visible names to their container (global/CoreForward, DisplayHandle, AssetHandle,
Event, SDL helpers), the binding helper style used, and the implementing C++ symbol
and source file where the binding is registered.

Notes
- The binding system uses sol2. Most functions are registered by small helper wrappers
  in `src/lua_BindHelpers.cpp` which attach three things where applicable: a method on
  the Core userdata, an entry in the `CoreForward` forwarding table, and a best-effort
  global alias (inside a try/catch so failures are non-fatal).
- Display- and Asset-level APIs are attached to shared, idempotent handle tables created
  by `DisplayHandle::ensure_handle_table(lua)` and `AssetHandle::ensure_handle_table(lua)`.
  Individual object types augment these shared tables with type-specific functions —
  modules always check for absence before setting a function so multiple modules can
  safely augment the same handle surface.

How to read entries
- Lua name —> Container (helper used) —> Implementing C++ symbol / registration site

## Globals / Core (lua `Core` / `CoreForward`)
All Core-level functions are registered in `Core::_registerLuaBindings` (file: `src/SDOM_Core.cpp`).
These use helpers in `src/lua_BindHelpers.cpp` which simultaneously register: Core usertype
method, `CoreForward` table entry, and sometimes a top-level global alias.

- `quit` —> Core/CoreForward (bind_noarg) —> `quit_lua` (registered in `src/SDOM_Core.cpp`)
- `shutdown` —> Core/CoreForward (bind_noarg) —> `shutdown_lua` (`src/SDOM_Core.cpp`)
- `configure` —> Core/CoreForward (bind_table) —> `configure_lua` (`src/SDOM_Core.cpp`)
- `configureFromFile` —> Core/CoreForward (bind_string) —> `configureFromFile_lua` (`src/SDOM_Core.cpp`)
- `configure` (global alias) —> top-level global `configure(table)` also set to convenience wrapper

- Callback registration helpers (callbacks forwarded to C++ event registrars):
  - `registerOnInit` —> (bind_callback_bool) —> `registerOnInit_lua` (`src/SDOM_Core.cpp`)
  - `registerOnQuit` —> (bind_callback_void) —> `registerOnQuit_lua` (`src/SDOM_Core.cpp`)
  - `registerOnUpdate` —> (bind_callback_update) —> `registerOnUpdate_lua` (`src/SDOM_Core.cpp`)
  - `registerOnEvent` —> (bind_callback_event) —> `registerOnEvent_lua` (`src/SDOM_Core.cpp`)
  - `registerOnRender` —> (bind_callback_void) —> `registerOnRender_lua` (`src/SDOM_Core.cpp`)
  - `registerOnUnitTest` —> (bind_callback_bool) —> `registerOnUnitTest_lua` (`src/SDOM_Core.cpp`)
  - `registerOnWindowResize` —> (bind_callback_resize) —> `registerOnWindowResize_lua` (`src/SDOM_Core.cpp`)
  - `registerOn` —> (bind_string_function_forwarder) —> `registerOn_lua` (`src/SDOM_Core.cpp`)

- Stage / root node management
  - `setRootNode`, `setRoot`, `setStage` —> (bind_name_or_handle) —> `setRootNodeByName_lua` / `setRootNode_lua` (`src/SDOM_Core.cpp`)
  - `getRoot`, `getRootHandle`, `getStage`, `getStageHandle` —> (bind_return_displayobject) —> `getRoot_lua` / `getStage_lua`

- Factory & object creation
  - `createDisplayObject(typeName, table)` —> (bind_string_table_return_do) —> `createDisplayObject_lua` (`src/SDOM_Core.cpp`)
  - `getDisplayObject(name)` —> (bind_string_return_do) —> `getDisplayObject_lua`
  - `hasDisplayObject(name)` —> (bind_string_return_bool) —> `hasDisplayObject_lua`
  - `createAssetObject(typeName, table)` and alias `createAsset` —> (bind_string_table_return_asset) —> `createAssetObject_lua`
  - `getAssetObject(name)` —> (bind_string_return_asset) —> `getAssetObject_lua`
  - `hasAssetObject(name)` —> (bind_string_return_bool) —> `hasAssetObject_lua`

- Focus & hover management
  - `doTabKeyPressForward`, `handleTabKeyPress` —> (bind_noarg) —> `doTabKeyPressForward_lua` (alias)
  - `doTabKeyPressReverse`, `handleTabKeyPressReverse` —> (bind_noarg) —> `doTabKeyPressReverse_lua`
  - `setKeyboardFocusedObject` —> (bind_do_arg) —> `setKeyboardFocusedObject_lua`
  - `getKeyboardFocusedObject` —> (bind_return_displayobject) —> `getKeyboardFocusedObject_lua`
  - `setMouseHoveredObject` —> (bind_do_arg) —> `setMouseHoveredObject_lua`
  - `getMouseHoveredObject` —> (bind_return_displayobject) —> `getMouseHoveredObject_lua`

- Window / timing / utility
  - `getWindowTitle` —> (bind_return_string) —> `getWindowTitle_lua`
  - `setWindowTitle(title)` —> (bind_string) —> `setWindowTitle_lua`
  - `getElapsedTime`, `getDeltaTime` —> (bind_return_float) —> `getElapsedTime_lua` (alias)
  - `pumpEventsOnce` —> (bind_noarg) —> `pumpEventsOnce_lua`
  - `pushMouseEvent`, `pushKeyboardEvent` —> (bind_object_arg) —> `pushMouseEvent_lua`, `pushKeyboardEvent_lua`
  - `destroyDisplayObject(name)` —> (bind_string) —> `destroyDisplayObject_lua`
  - `collectGarbage` —> (bind_noarg) —> `collectGarbage_lua`
  - `printObjectRegistry` —> (bind_noarg) —> `printObjectRegistry_lua`

For the full Core function list, see `src/SDOM_Core.cpp` inside `_registerLuaBindings` where each call to `SDOM::bind_*` shows the mapping.

## DisplayHandle methods (registered by `IDisplayObject::_registerLuaBindings`)
Registration site: `src/SDOM_IDisplayObject.cpp`. All methods are attached to the shared
handle table `DisplayHandle` returned by `DisplayHandle::ensure_handle_table(lua)`.
Each method registration uses an `absent()` guard so augmentation is idempotent.

Categories and representative methods (Lua name —> wrapper style —> C++ symbol):

- Event handling
  - `addEventListener` —> sol::overload(...) —> `::SDOM::addEventListener_lua` / `addEventListener_lua_any` / `addEventListener_lua_any_short` (`src/SDOM_IDisplayObject.cpp`)
  - `removeEventListener` —> sol::overload(...) —> `::SDOM::removeEventListener_lua` / `removeEventListener_lua_any` / `removeEventListener_lua_any_short`

- State & debug
  - `cleanAll` —> bind_void_0 —> `::SDOM::cleanAll_lua`
  - `getDirty` —> bind_R_0 —> `::SDOM::getDirty_lua`
  - `setDirty` —> bind_void_0 —> `::SDOM::setDirty_lua`
  - `isDirty` —> bind_R_0 —> `::SDOM::isDirty_lua`
  - `printTree` —> bind_void_0 —> `::SDOM::printTree_lua`

- Hierarchy (children/parent)
  - `addChild` —> set_function (lambda) —> `::SDOM::addChild_lua`
  - `removeChild(name | handle)` —> sol::overload(...) —> `::SDOM::removeChild_lua`
  - `hasChild(handle)` —> bind_R_do —> `::SDOM::hasChild_lua`
  - `getChild(name)` —> bind_R_str —> `::SDOM::getChild_lua`
  - `getParent` —> bind_R_0 —> `::SDOM::getParent_lua`
  - `setParent(handle)` —> bind_void_do —> `::SDOM::setParent_lua`
  - `removeFromParent` —> bind_R_0_nc —> `::SDOM::removeFromParent_lua`
  - `removeDescendant`, `removeDescendantByName` —> bind_R_do_nc / bind_R_str_nc —> `::SDOM::removeDescendant_lua`

- Type & properties
  - `getName` —> custom lambda (tries live object, falls back to handle cached name) —> `::SDOM::getName_lua` / `::SDOM::getName_handle_lua`
  - `setName` —> bind_void_str —> `::SDOM::setName_lua`
  - `getType` —> bind_R_0 —> `::SDOM::getType_lua`
  - `setType` —> bind_void_str —> `::SDOM::setType_lua`
  - `getBounds` —> bind_R_0 —> `::SDOM::getBounds_lua`
  - `setBounds` —> bind_void_obj —> `::SDOM::setBounds_lua`
  - `getColor` —> bind_R_0 —> `::SDOM::getColor_lua`
  - `setColor` —> bind_void_obj —> `::SDOM::setColor_lua`

- Priority & Z-order
  - `getMaxPriority`, `getMinPriority`, `getPriority` —> bind_R_0 —> corresponding `::SDOM::*_lua`
  - `setToHighestPriority`, `setToLowestPriority`, `setPriority` —> sol::overload wrappers —> `::SDOM::set*` helpers
  - `sortChildrenByPriority`, `getChildrenPriorities`, `moveToTop`, `getZOrder`, `setZOrder` —> various bind_* helpers and overloads

- Focus & interactivity
  - `setKeyboardFocus`, `isKeyboardFocused`, `isMouseHovered`, `isClickable`, `setClickable`, `isEnabled`, `setEnabled`, `isHidden`, `setHidden`, `isVisible`, `setVisible` —> mix of bind_void_0, bind_R_0, bind_void_b —> `::SDOM::*_lua`

- Tab management
  - `getTabPriority`, `setTabPriority`, `isTabEnabled`, `setTabEnabled` —> bind_R_0 / bind_void_i / bind_void_b —> `::SDOM::*_lua`

- Geometry & layout
  - `getX`, `getY`, `getWidth`, `getHeight`, `setX`, `setY`, `setWidth`, `setHeight` —> bind_R_0 / bind_void_i —> `::SDOM::*_lua`
  - Anchor getters/setters (`getAnchorTop`, `setAnchorTop`, etc.) —> bind_R_0 / bind_void_ap —> `::SDOM::*_lua`
  - World-local edge getters/setters: `getLeft`/`setLeft`, `getRight`/`setRight`, `getTop`/`setTop`, `getBottom`/`setBottom` —> bind_R_0 / bind_void_f —> `::SDOM::*_lua`

- Orphan retention helpers
  - `orphanPolicyFromString`, `orphanPolicyToString`, `setOrphanRetentionPolicy`, `getOrphanRetentionPolicyString`, `getOrphanGrace`, `setOrphanGrace` —> bind_R_str / bind_R_orp / bind_void_str / bind_R_0 / bind_void_i —> `::SDOM::*_lua`

Note: each `::SDOM::*_lua` symbol is defined in the C++ codebase (many in `src/SDOM_IDisplayObject.cpp` or `src/lua_IDisplayObject.hpp` wrappers). For a line-by-line mapping, search for `handle.set_function(` and the function names in `src/SDOM_IDisplayObject.cpp`.

## Exhaustive DisplayHandle mapping (line-by-line)

Below is a complete, ordered mapping of every `handle.set_function(...)` registration found in `src/SDOM_IDisplayObject.cpp` (the display-handle augmentations). Each entry shows: Lua method name → registration style / helper → implementing C++ symbol(s) and source file where the wrapper is defined.

(Annotated file:line references)

- `addEventListener` — sol::overload(...) — registration: `src/SDOM_IDisplayObject.cpp:1500` — implementations: `SDOM::addEventListener_lua` (src/lua_IDisplayObject.cpp:22), `SDOM::addEventListener_lua_any` (src/lua_IDisplayObject.cpp:47), `SDOM::addEventListener_lua_any_short` (src/lua_IDisplayObject.cpp:146)
- `removeEventListener` — sol::overload(...) — registration: `src/SDOM_IDisplayObject.cpp:1515` — implementations: `SDOM::removeEventListener_lua` (src/lua_IDisplayObject.cpp:33), `SDOM::removeEventListener_lua_any` (src/lua_IDisplayObject.cpp:99), `SDOM::removeEventListener_lua_any_short` (src/lua_IDisplayObject.cpp:150)

- `cleanAll` — bind_void_0 — registration: `src/SDOM_IDisplayObject.cpp:1529` — implementation: `SDOM::cleanAll_lua` (src/lua_IDisplayObject.cpp:13)
- `getDirty` — bind_R_0 — registration: `src/SDOM_IDisplayObject.cpp:1530` — implementation: `SDOM::getDirty_lua` (src/lua_IDisplayObject.cpp:14)
- `setDirty` — bind_void_0 — registration: `src/SDOM_IDisplayObject.cpp:1531` — implementation: `SDOM::setDirty_lua` (src/lua_IDisplayObject.cpp:15)
- `isDirty` — bind_R_0 — registration: `src/SDOM_IDisplayObject.cpp:1532` — implementation: `SDOM::isDirty_lua` (src/lua_IDisplayObject.cpp:16)
- `printTree` — bind_void_0 — registration: `src/SDOM_IDisplayObject.cpp:1533` — implementation: `SDOM::printTree_lua` (src/lua_IDisplayObject.cpp:19)

- `addChild` — set_function(lambda) — registration: `src/SDOM_IDisplayObject.cpp:1536` — implementation wrapper: lambda calls `SDOM::addChild_lua` (`src/lua_IDisplayObject.cpp:155` defines `addChild_lua`)
- `removeChild` — sol::overload(name | handle) — registration: `src/SDOM_IDisplayObject.cpp:1539` — implementation: `SDOM::removeChild_lua` (src/lua_IDisplayObject.cpp:157-158)
- `hasChild` — bind_R_do — registration: `src/SDOM_IDisplayObject.cpp:1544` — implementation: `SDOM::hasChild_lua` (src/lua_IDisplayObject.cpp:159)
- `getChild` — bind_R_str — registration: `src/SDOM_IDisplayObject.cpp:1545` — implementation: `SDOM::getChild_lua` (src/lua_IDisplayObject.cpp:156)
- `getParent` — bind_R_0 — registration: `src/SDOM_IDisplayObject.cpp:1546` — implementation: `SDOM::getParent_lua` (src/lua_IDisplayObject.cpp:160)
- `setParent` — bind_void_do — registration: `src/SDOM_IDisplayObject.cpp:1547` — implementation: `SDOM::setParent_lua` (src/lua_IDisplayObject.cpp:161)
- `isAncestorOf` — bind_R_do_nc — registration: `src/SDOM_IDisplayObject.cpp:1548` — implementation: `SDOM::isAncestorOf_lua` (src/lua_IDisplayObject.cpp:164)
- `isAncestorOfName` — bind_R_str_nc — registration: `src/SDOM_IDisplayObject.cpp:1549` — implementation: `SDOM::isAncestorOf_lua` (string overload) (src/lua_IDisplayObject.cpp:165)
- `isDescendantOf` — bind_R_do_nc — registration: `src/SDOM_IDisplayObject.cpp:1550` — implementation: `SDOM::isDescendantOf_lua` (src/lua_IDisplayObject.cpp:166)
- `isDescendantOfName` — bind_R_str_nc — registration: `src/SDOM_IDisplayObject.cpp:1551` — implementation: `SDOM::isDescendantOf_lua` (string overload) (src/lua_IDisplayObject.cpp:167)
- `removeFromParent` — bind_R_0_nc — registration: `src/SDOM_IDisplayObject.cpp:1552` — implementation: `SDOM::removeFromParent_lua` (src/lua_IDisplayObject.cpp:168)
- `removeChildByHandle` — bind_R_do_nc — registration: `src/SDOM_IDisplayObject.cpp:1553` — implementation: `SDOM::removeChild_lua` (handle overload) (src/lua_IDisplayObject.cpp:157)
- `removeChildByName` — bind_R_str_nc — registration: `src/SDOM_IDisplayObject.cpp:1554` — implementation: `SDOM::removeChild_lua` (name overload) (src/lua_IDisplayObject.cpp:158)
- `removeDescendant` — bind_R_do_nc — registration: `src/SDOM_IDisplayObject.cpp:1555` — implementation: `SDOM::removeDescendant_lua` (src/lua_IDisplayObject.cpp:169)
- `removeDescendantByName` — bind_R_str_nc — registration: `src/SDOM_IDisplayObject.cpp:1556` — implementation: `SDOM::removeDescendant_lua` (name overload) (src/lua_IDisplayObject.cpp:170)


 - `getName` — custom lambda — registration: `src/SDOM_IDisplayObject.cpp:1561` — implementations: `SDOM::getName_lua` (src/lua_IDisplayObject.cpp:173), `SDOM::getName_handle_lua` (src/lua_IDisplayObject.cpp:293)
 - `setName` — bind_void_str — registration: `src/SDOM_IDisplayObject.cpp:1567` — implementation: `SDOM::setName_lua` (src/lua_IDisplayObject.cpp:174)
 - `getType` — bind_R_0 — registration: `src/SDOM_IDisplayObject.cpp:1568` — implementation: `SDOM::getType_lua` (src/lua_IDisplayObject.cpp:175)
 - `setType` — bind_void_str — registration: `src/SDOM_IDisplayObject.cpp:1569` — implementation: `SDOM::setType_lua` (src/lua_IDisplayObject.cpp:176)
 - `getBounds` — bind_R_0 — registration: `src/SDOM_IDisplayObject.cpp:1570` — implementation: `SDOM::getBounds_lua` (src/lua_IDisplayObject.cpp:177)
 - `setBounds` — bind_void_obj — registration: `src/SDOM_IDisplayObject.cpp:1571` — implementation: `SDOM::setBounds_lua` (src/lua_IDisplayObject.cpp:216 / overloaded at src/lua_IDisplayObject.cpp:257)
 - `getColor` — bind_R_0 — registration: `src/SDOM_IDisplayObject.cpp:1572` — implementation: `SDOM::getColor_lua` (src/lua_IDisplayObject.cpp:258)
 - `setColor` — bind_void_obj — registration: `src/SDOM_IDisplayObject.cpp:1573` — implementation: `SDOM::setColor_lua` (src/lua_IDisplayObject.cpp:260 / overloaded at src/lua_IDisplayObject.cpp:290)

- `getMaxPriority` — bind_R_0 — registration: `src/SDOM_IDisplayObject.cpp:1576` — implementation: `SDOM::getMaxPriority_lua` (src/lua_IDisplayObject.cpp:305)
- `getMinPriority` — bind_R_0 — registration: `src/SDOM_IDisplayObject.cpp:1577` — implementation: `SDOM::getMinPriority_lua` (src/lua_IDisplayObject.cpp:306)
- `getPriority` — bind_R_0 — registration: `src/SDOM_IDisplayObject.cpp:1578` — implementation: `SDOM::getPriority_lua` (src/lua_IDisplayObject.cpp:307)


 - `sortChildrenByPriority` — bind_void_0 — registration: `src/SDOM_IDisplayObject.cpp:1580` — implementation: `SDOM::sortChildrenByPriority_lua` (src/lua_IDisplayObject.cpp:310)
 - `setToHighestPriority` — sol::overload — registration: `src/SDOM_IDisplayObject.cpp:1582-1584` — implementation: `SDOM::setToHighestPriority_lua` (src/lua_IDisplayObject.cpp:308), `SDOM::setToHighestPriority_lua_any` (src/lua_IDisplayObject.cpp:448)
 - `setToLowestPriority` — sol::overload — registration: `src/SDOM_IDisplayObject.cpp:1588-1590` — implementation: `SDOM::setToLowestPriority_lua` (src/lua_IDisplayObject.cpp:309), `SDOM::setToLowestPriority_lua_any` (src/lua_IDisplayObject.cpp:472)
 - `setPriority` — sol::overload — registration: `src/SDOM_IDisplayObject.cpp:1594-1597` — implementation: `SDOM::setPriority_lua` (src/lua_IDisplayObject.cpp:311), `SDOM::setPriority_lua_any` (src/lua_IDisplayObject.cpp:323), `SDOM::setPriority_lua_target` (src/lua_IDisplayObject.cpp:344)
 - `getChildrenPriorities` — bind_R_0 — registration: `src/SDOM_IDisplayObject.cpp:1600` — implementation: `SDOM::getChildrenPriorities_lua` (src/lua_IDisplayObject.cpp:370)
 - `moveToTop` — sol::overload — registration: `src/SDOM_IDisplayObject.cpp:1602-1604` — implementation: `SDOM::moveToTop_lua` (src/lua_IDisplayObject.cpp:376), `SDOM::moveToTop_lua_any` (src/lua_IDisplayObject.cpp:384)
 - `getZOrder` — bind_R_0 — registration: `src/SDOM_IDisplayObject.cpp:1607` — implementation: `SDOM::getZOrder_lua` (src/lua_IDisplayObject.cpp:408)
 - `setZOrder` — sol::overload — registration: `src/SDOM_IDisplayObject.cpp:1609-1611` — implementation: `SDOM::setZOrder_lua` (src/lua_IDisplayObject.cpp:409), `SDOM::setZOrder_lua_any` (src/lua_IDisplayObject.cpp:412)


Notes:
- Nearly every wrapper referenced above is implemented in `src/lua_IDisplayObject.cpp` (this file contains the user-facing Lua wrappers named with the `_lua` suffix). The `DisplayHandle::bind_minimal` helper (in `src/SDOM_DisplayHandle.cpp`) registers the minimal handle surface (`isValid`, `getName`, `getType`, `setName`, `setType`) where `getName`/`getType` resolution for raw handles is implemented in `include/SDOM/SDOM_DisplayHandle.hpp` as small inline adapters (`getName_lua`, `getType_lua`).

## AssetHandle and per-asset augmentations
Minimal AssetHandle bindings (idempotent) are registered in `src/SDOM_AssetHandle.cpp` via `AssetHandle::bind_minimal`:

- `isValid` —> `AssetHandle::isValid`
- `getName` —> `AssetHandle::getName_lua`
- `getType` —> `AssetHandle::getType_lua`
- `getFilename` —> `AssetHandle::getFilename_lua`
- `setName`, `setType`, `setFilename` —> simple lambdas that set handle fields

Per-asset augmentations (examples)
- Texture (`src/SDOM_Texture.cpp`)
  - `getTextureWidth` —> lambda (casts `AssetHandle` -> `Texture*`) —> `Texture::getTextureWidth()`
  - `getTextureHeight` —> lambda (casts `AssetHandle` -> `Texture*`) —> `Texture::getTextureHeight()`

- Truetype/TTF assets (`src/SDOM_TTFAsset.cpp`, `src/SDOM_TruetypeFont.cpp`)
  - The files contain binding skeletons; pattern is to augment `AssetHandle` with font-specific helpers (e.g., drawGlyph/drawPhrase, glyph metrics). In the current code these are organized inside the asset classes and expected to register into `AssetHandle` (check `SDOM_TruetypeFont.cpp` and `SDOM_TTFAsset.cpp` for exact symbol names).

- Label (`src/SDOM_Label.cpp`)
  - `Label` contains rendering and layout logic and a binding stub; label-specific methods are expected to augment `DisplayHandle` (the binding in the file is commented-out and the handle-augmentation pattern is preferred). See `src/SDOM_Label.cpp` for helper names like `setText`, `getText` and texture rebuild methods.

## Event and EventType bindings
Registration: `src/SDOM_Event.cpp`

- `Event` usertype (lua `Event`) registered with properties and methods including:
  - `dt` (property) —> event elapsed time
  - `type` (property) —> `getTypeName()` (string)
  - `target` (property) —> `getTarget()` (DisplayHandle)
  - `sdl` (property) —> returns Lua table conversion of underlying `SDL_Event` via `SDL_Utils::eventToLuaTable`
  - `getPayload` / `setPayload` —> payload accessors (safe; `getPayload` returns a table snapshot)
  - Many typed accessors: `getMouseX`, `getMouseY`, `getKeycode`, `getScanCode`, `getAsciiCode`, `stopPropagation`, etc.
  - `pairs` meta-function implemented to iterate a small snapshot set: `type`, `dt`, `target` (so `for k,v in pairs(ev) do ... end` yields these keys reliably)

- `EventType` usertype is registered and a read-only `EventType` table is populated in `Core::_registerLuaBindings` from the C++ EventType registry.

## SDL helpers (lua `SDL_Utils` / `SDL`)
Registration: `src/SDOM_SDL_Utils.cpp`

- `SDL_Color` usertype is exposed with factories and r/g/b/a fields plus `fromTable` helper.
- `SDL_Utils` table (also exposed as `SDL`) contains:
  - `pixelFormatToString`, `pixelFormatFromString`
  - `windowFlagsToString`, `windowFlagsFromString`
  - `rendererLogicalPresentationToString`, `rendererLogicalPresentationFromString`
  - `eventTypeToString`, `eventTypeFromString`
  - `keyToAscii` —> helper that maps SDL keycodes/mods to ASCII
  - `Delay` —> wrapper for `SDL_Delay` (supports both table:method and function call forms)
  - `eventToLuaTable` —> converts `SDL_Event` to a Lua table (used by Event.sdl property)

## Binding helper summary (src/lua_BindHelpers.cpp)
The helpers centralize registration semantics and reduce duplication. Important helper behaviors:

- bind_noarg(name, func, objHandleType, coreTable, lua)
  - Registers a Core usertype method taking no args, a `coreTable` forwarding function, and attempts a top-level `lua[name]` alias.

- bind_table, bind_string, bind_bool_arg, bind_object_arg, bind_do_arg
  - Variants that accept different arg shapes (table, string, bool, sol::object, DisplayHandle resolver) and register onto Core usertype and forwarding table.

- bind_return_* helpers
  - Register Core methods that return DisplayHandle, bool, string, float, int, vector<DisplayHandle>, vector<string>.

- bind_callback_* helpers
  - Helpers that take a registrar function to accept Lua callbacks and convert them to safe C++ closures (protected_function wrappers with error logging). Used for registerOn* APIs.

- bind_name_or_handle
  - Registers a forwarding function that accepts either a name (string/table) or a DisplayHandle and dispatches to nameFunc or handleFunc accordingly. Core uses this for convenience setRoot/setStage helpers.

See `src/lua_BindHelpers.cpp` for precise conversion semantics and examples.

## Remaining work / completeness notes
- This document covers Core-global functions, the DisplayHandle surface (categories + examples), AssetHandle minimal bindings and a sample per-asset augmentation (Texture), Event and SDL helper bindings, and the binding helper behaviors.
- Remaining tasks to reach an exhaustive, line-by-line mapping:
  1. Finish enumerating every `handle.set_function(...)` entry in `src/SDOM_IDisplayObject.cpp` and list the exact Lua name → implementing symbol line-by-line (TODO: currently summarized by category).
  2. Scan per-asset modules (`src/SDOM_*.cpp`) for asset-specific augmentations and record each asset API (e.g., SpriteSheet, BitmapFont, TruetypeFont methods).
  3. Sweep the repository for any ad-hoc `lua.new_usertype`, `set_function`, or `lua.set_function` calls in examples and extensions and add them to this doc.


---
Completion summary
- Files read while producing this doc: `src/lua_BindHelpers.cpp`, `src/SDOM_Core.cpp`, `src/SDOM_IDisplayObject.cpp`, `src/SDOM_DisplayHandle.cpp`, `src/SDOM_AssetHandle.cpp`, `src/SDOM_Event.cpp`, `src/SDOM_SDL_Utils.cpp`, `src/SDOM_Texture.cpp`, `src/SDOM_TTFAsset.cpp`, `src/SDOM_TruetypeFont.cpp`, `src/SDOM_Label.cpp`, `examples/test/src/Box.cpp`.
