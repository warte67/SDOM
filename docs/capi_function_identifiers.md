Core C/C++ Identifier Map
=========================

The table below enumerates every publicly declared member of `SDOM::Core` and aligns each method with its C API identifier (existing or proposed) and Lua binding name. Use this as the master checklist while expanding the generated bindings so that the Core singleton exposes a consistent surface area across C++, the `SDOM_*` C APIs, and Lua (`Core:<Method>` idiom).

Legend
------
- **Existing** – already implemented in `SDOM_CoreAPI.cpp` and surfaced through the generated registry.
- **Proposed** – identifier still needs generator support; name follows the preferred `SDOM_<Verb>` scheme unless compatibility requires otherwise.
- **Internal** – framework lifecycle hooks that should stay C++-only.

> **Naming rule:** Core C API functions must not repeat the word “Core.” Use `SDOM_<Verb>` (e.g., `SDOM_Reconfigure`, `SDOM_SetStage`) because the singleton context is implied once you invoke the SDOM prefix.
> **Signature guidance:** Prefer the common `bool` return value for pass/fail, but treat it as a guideline—capability queries like `SDOM_HasLuaSupport()` or state getters such as `SDOM_IsWindowed()` / `SDOM_IsFullscreen()` may return `true`/`false` strictly to report availability or mode, not success/failure.

Lifecycle & Main Loop
---------------------

| C++ API | C API Identifier | Lua Binding | Status | Notes |
| --- | --- | --- | --- | --- |
| `static Core& getInstance()` | _n/a_ | _n/a_ | Internal | Singleton access; C API uses free functions instead of exposing the instance pointer. |
| `bool onInit()` | _callback only_ | _callback only_ | Internal | Scripts should register via `RegisterOnInit`; direct Lua method is intentionally withheld. |
| `void onQuit()` | _callback only_ | _callback only_ | Internal | Use `RegisterOnQuit`; direct Lua access removed in favor of callbacks. |
| `bool onUnitTest(int frame)` | _callback only_ | _callback only_ | Internal | Triggered through `RegisterOnUnitTest`; not exported as a callable Lua method. |
| `bool run()` | `SDOM_Run` | `Core:Run()` | Existing | Enters the main loop until quit/stop is requested. |
| `void isRunning(bool running)` | `SDOM_SetIsRunning` | `Core:SetIsRunning(running)` | Existing | Allows scripts/tests to pause the loop without a full quit. |
| `void quit()` | `SDOM_Quit` | `Core:Quit()` | Existing | Graceful shutdown; also aliased by `shutdown()`. |
| `void shutdown()` | `SDOM_Quit` | `Core:Shutdown()` | Existing | Thin wrapper over `quit()`; reuse the same C API entry point. |
| `void onRender()` | _callback only_ | _callback only_ | Internal | Use `RegisterOnRender`; the Lua runtime should not call this directly. |
| `void onEvent(Event& evt)` | _callback only_ | _callback only_ | Internal | Events are surfaced via listener registration instead of a direct Lua method. |
| `void onUpdate(float dt)` | _callback only_ | _callback only_ | Internal | Use `RegisterOnUpdate` to hook into the frame loop. |
| `void onWindowResize(int w, int h)` | `SDOM_OnWindowResize` | _callback only_ | Internal | Delivered through `RegisterOnWindowResize` rather than exposing `Core:OnWindowResize()` to Lua. |

Configuration & JSON Project Loading
------------------------------------

| C++ API | C API Identifier | Lua Binding | Status | Notes |
| --- | --- | --- | --- | --- |
| `void configure(const CoreConfig& cfg)` | `SDOM_Configure` | `Core:Configure(cfg)` | Existing | Applies a fresh configuration prior to SDL startup. |
| `bool configureFromJson(const nlohmann::json& doc)` | `SDOM_ConfigureFromJson` | `Core:ConfigureFromJson(json)` | Existing | Converts JSON blobs to `CoreConfig`. |
| `bool loadDomFromJson(const nlohmann::json& doc)` | `SDOM_LoadDomFromJson` | `Core:LoadDomFromJson(json)` | Existing | Convenience that chains configure → DOM build; JSON-first entry for in-memory documents. |
| `bool loadDomFromJsonFile(const std::string& path)` | `SDOM_LoadDomFromJsonFile` | `Core:LoadDomFromJsonFile(path)` | Existing | Primary entry point for variant 3/4 bootstraps (file-based JSON). |

Callback Registration & Lua-facing Helpers
------------------------------------------

| C++ API | C API Identifier | Lua Binding | Status | Notes |
| --- | --- | --- | --- | --- |
| `void registerOnInit(std::function<bool()>)` | `SDOM_RegisterOnInit` | `Core:RegisterOnInit(fn)` | Existing | Allows C/lua to inject callbacks executed before the main loop starts. |
| `void registerOnQuit(std::function<void()>)` | `SDOM_RegisterOnQuit` | `Core:RegisterOnQuit(fn)` | Existing | Invoke custom teardown logic. |
| `void registerOnUpdate(std::function<void(float)>)` | `SDOM_RegisterOnUpdate` | `Core:RegisterOnUpdate(fn)` | Existing | Frame-update hook. |
| `void registerOnEvent(std::function<void(const Event&)>)` | `SDOM_RegisterOnEvent` | `Core:RegisterOnEvent(fn)` | Existing | Allows listeners to observe raw SDL events. |
| `void registerOnRender(std::function<void()>)` | `SDOM_RegisterOnRender` | `Core:RegisterOnRender(fn)` | Existing | Custom render callback. |
| `void registerOnUnitTest(std::function<bool()>)` | `SDOM_RegisterOnUnitTest` | `Core:RegisterOnUnitTest(fn)` | Existing | Enables scripted tests to run alongside C++ suites. |
| `void registerOnWindowResize(std::function<void(int,int)>)` | `SDOM_RegisterOnWindowResize` | `Core:RegisterOnWindowResize(fn)` | Existing | Resize notifications for overlays. |
| `_fnOn*/_fnGetOn* helpers (seven setter/getter pairs)` | _n/a_ | `Core:_fnOnInit(fn)` etc. | Internal | Convenience shims reserved for Lua binding internals; keep C++-only. |

> **Lifecycle policy:** Both the Lua runtime *and* the public C API must wire lifecycle behavior exclusively through these callback registrars. Functions like `Core:onInit()` or `SDOM_OnWindowResize()` stay internal and are triggered by the engine, never invoked directly by external callers.

Stage / Root DOM Management
---------------------------

| C++ API | C API Identifier | Lua Binding | Status | Notes |
| --- | --- | --- | --- | --- |
| `void setRootNode(const std::string& name)` | `SDOM_SetRootNodeByName` | `Core:SetRootNode(name)` | Existing | Resolves a display handle by name and sets it as the stage root. |
| `void setRootNode(const DisplayHandle& handle)` | `SDOM_SetRootNode` | `Core:SetRootNodeHandle(handle)` | Existing | Direct-handle overload. |
| `void setStage(const std::string& name)` | `SDOM_SetStageByName` | `Core:SetStage(name)` | Existing | Alias retained for backward compatibility. |
| `void setRootNode(const DisplayHandle& handle)` | `SDOM_SetStage` | `Core:SetStageHandle(handle)` | Existing | Alias to set the stage via handle. |
| `Stage* getStage() const` | _n/a_ | `Core:GetStage()` | Internal | C API exposes handle-based getters instead. |
| `IDisplayObject* getRootNodePtr() const` | _n/a_ | `Core:GetRootNodePtr()` | Internal | Raw pointer unsafe for the public C API; expose as handle via `getRootNode()` instead. |
| `DisplayHandle getRootNode() const` | `SDOM_GetRootNode` | `Core:GetRootNode()` | Existing | Returns the active root/stage handle via out-param. |
| `DisplayHandle getStageHandle() const` | `SDOM_GetStageHandle` | `Core:GetStageHandle()` | Existing | Alias that matches older naming. |

SDL Resource Access & Presentation State
----------------------------------------

| C++ API | C API Identifier | Lua Binding | Status | Notes |
| --- | --- | --- | --- | --- |
| `SDL_Window* getWindow() const` | `SDOM_GetWindow()` | `Core:GetWindow()` | Existing | Caveat: This pointer is ABI Safe as long as SDL_DestroyWindow() is not allowed. |
| `SDL_Renderer* getRenderer() const` | `SDOM_GetRenderer()` | `Core:GetRenderer()` | Existing | Caveat: This pointer is ABI Safe as long as SDL_DestroyRenderer() is not allowed. |
| `SDL_Texture* getTexture() const` | `SDOM_GetTexture()` | `Core:GetTexture()` | Existing | Caveat: This pointer is ABI Safe as long as SDL_DestroyTexture() is not allowed. |
| `SDL_Color getColor() const` | `SDOM_GetBorderColor` | `Core:GetColor()` | Existing | Returns the frame border color applied during presentation. |
| `void setColor(const SDL_Color& color)` | `SDOM_SetBorderColor` | `Core:SetColor(color)` | Existing | Updates the border color and pushes it to the renderer immediately. |

> Ownership note: SDL resources returned by Core (window/renderer/texture) are **SDOM-owned borrows**. Callers may use them with SDL APIs but must **never destroy** (`SDL_Destroy*`) or reparent them unless the resource was created via a CAPI call that explicitly transfers ownership. Destroy only what you create.

Window Title & Timing
---------------------

| C++ API | C API Identifier | Lua Binding | Status | Notes |
| --- | --- | --- | --- | --- |
| `std::string getWindowTitle() const` | `SDOM_GetWindowTitle` | `Core:GetWindowTitle()` | Existing | Returns cached title; in C return `const char*`. |
| `void setWindowTitle(const std::string& title)` | `SDOM_SetWindowTitle` | `Core:SetWindowTitle(title)` | Existing | Calls `SDL_SetWindowTitle` when live. |
| `float getElapsedTime() const` | `SDOM_GetElapsedTime` | `Core:GetElapsedTime()` | Existing | Last frame delta time in seconds. |

Configuration Accessors (Getters)
---------------------------------

| C++ API | C API Identifier | Lua Binding | Status | Notes |
| --- | --- | --- | --- | --- |
| `float getWindowWidth() const` | `SDOM_GetWindowWidth` | `Core:GetWindowWidth()` | Proposed | Convenience scalar accessor. |
| `float getWindowHeight() const` | `SDOM_GetWindowHeight` | `Core:GetWindowHeight()` | Proposed | — |
| `float getPixelWidth() const` | `SDOM_GetPixelWidth` | `Core:GetPixelWidth()` | Proposed | — |
| `float getPixelHeight() const` | `SDOM_GetPixelHeight` | `Core:GetPixelHeight()` | Proposed | — |
| `bool getPreserveAspectRatio() const` | `SDOM_GetPreserveAspectRatio` | `Core:GetPreserveAspectRatio()` | Proposed | — |
| `bool getAllowTextureResize() const` | `SDOM_GetAllowTextureResize` | `Core:GetAllowTextureResize()` | Proposed | — |
| `SDL_RendererLogicalPresentation getRendererLogicalPresentation() const` | `SDOM_GetLogicalPresentation` | `Core:GetRendererLogicalPresentation()` | Proposed | Mirrors SDL logical presentation enum. |
| `SDL_WindowFlags getWindowFlags() const` | `SDOM_GetWindowFlags` | `Core:GetWindowFlags()` | Proposed | — |
| `SDL_PixelFormat getPixelFormat() const` | `SDOM_GetPixelFormat` | `Core:GetPixelFormat()` | Proposed | — |
| `bool isFullscreen() const` | `SDOM_IsFullscreen` | `Core:IsFullscreen()` | Proposed | Derived from SDL window state. |
| `bool isWindowed() const` | `SDOM_IsWindowed` | `Core:IsWindowed()` | Proposed | Complement of `IsFullscreen`. |
| `bool getIsTraversing() const` | `SDOM_GetIsTraversing` | `Core:GetIsTraversing()` | Proposed | Reports when the DOM is currently being traversed; safe for tests & tooling. |
| `void setIsTraversing(bool)` | `SDOM_SetIsTraversing` | `Core:SetIsTraversing(flag)` | Proposed | Allows tests or async loaders to suppress traversal during mutation. |

Configuration Mutators (Setters)
--------------------------------

| C++ API | C API Identifier | Lua Binding | Status | Notes |
| --- | --- | --- | --- | --- |
| `void setWindowWidth(float width)` | `SDOM_SetWindowWidth` | `Core:SetWindowWidth(width)` | Proposed | Auto-triggers reconfigure when SDL is active. |
| `void setWindowHeight(float height)` | `SDOM_SetWindowHeight` | `Core:SetWindowHeight(height)` | Proposed | — |
| `void setPixelWidth(float width)` | `SDOM_SetPixelWidth` | `Core:SetPixelWidth(width)` | Proposed | — |
| `void setPixelHeight(float height)` | `SDOM_SetPixelHeight` | `Core:SetPixelHeight(height)` | Proposed | — |
| `void setPreserveAspectRatio(bool preserve)` | `SDOM_SetPreserveAspectRatio` | `Core:SetPreserveAspectRatio(preserve)` | Proposed | — |
| `void setAllowTextureResize(bool allow)` | `SDOM_SetAllowTextureResize` | `Core:SetAllowTextureResize(allow)` | Proposed | — |
| `void setRendererLogicalPresentation(SDL_RendererLogicalPresentation)` | `SDOM_SetLogicalPresentation` | `Core:SetRendererLogicalPresentation(mode)` | Proposed | — |
| `void setWindowFlags(SDL_WindowFlags flags)` | `SDOM_SetWindowFlags` | `Core:SetWindowFlags(flags)` | Proposed | Consider exposing explicit helpers for fullscreen toggles. |
| `void setPixelFormat(SDL_PixelFormat fmt)` | `SDOM_SetPixelFormat` | `Core:SetPixelFormat(fmt)` | Proposed | Requires texture rebuild. |
| `void setFullscreen(bool fullscreen)` | `SDOM_SetFullscreen` | `Core:SetFullscreen(fullscreen)` | Proposed | Toggles SDL’s fullscreen mode; window flags/menu hints are ignored while fullscreen is active. |
| `void setWindowed(bool windowed)` | `SDOM_SetWindowed` | `Core:SetWindowed(windowed)` | Proposed | Complements `SetFullscreen`; only two modes exist so this simply clears fullscreen and re-applies standard window flags. |

Factory / Registry / Traversal State
====================================

This section contains **only** the Core functions that are appropriate for
exposure to external languages (C API, Lua).  
All internal engine subsystems — Factory, EventManager, DataRegistry,
and the BindGenerator — remain strictly inside the C++ implementation.

Only **engine-state queries** from this category are suitable for
public binding. Everything else is for internal Core operation and
must not be surfaced outside C++.

Public API (External Bindings)
------------------------------

| C++ API                      | C API Identifier        | Lua Binding                 | Status    | Notes |
|-----------------------------|-------------------------|-----------------------------|-----------|-------|
| `bool getIsTraversing() const` | `SDOM_GetIsTraversing` | `Core:GetIsTraversing()`     | Proposed  | Reports when the DOM is currently being traversed; safe for tests & tooling. |
| `void setIsTraversing(bool)`   | `SDOM_SetIsTraversing` | `Core:SetIsTraversing(flag)` | Proposed  | Allows tests or async loaders to suppress traversal during mutation. |

Internal-Only (Not for CAPI, Not for Lua)
-----------------------------------------

These functions are **engine infrastructure**, tightly bound to the internal
implementation and not ABI-safe. They must **not** appear in generated
bindings.

| C++ API                               | Reason for Exclusion |
|---------------------------------------|-----------------------|
| `Factory& getFactory() const`         | Exposes object-creation internals; bypasses safety. |
| `EventManager& getEventManager() const` | Low-level event pipeline; volatile to refactoring. |
| `DataRegistry& getDataRegistry()`     | Reflection/compiler subsystem; unsafe for foreign code. |
| `DataRegistry& getRegistry()` (deprecated) | Deprecated internal alias. |
| `bool exportBindings()`               | Invokes BindGenerator; can overwrite SDK output. |

Rationale
---------

Only traversal-state queries (`getIsTraversing`, `setIsTraversing`)
represent a stable, safe, engine-level status flag.  
All other items in this category operate on **engine infrastructure**
that external callers must never touch.

This separation preserves:

- ABI stability  
- soft real-time safety  
- DOM integrity during mutation  
- the flexibility to refactor internals without breaking the C API


Focus & Hover Management
========================

These APIs control SDOM’s **keyboard focus chain** and **mouse hover state**.
They are safe to expose (they use only `DisplayHandle`), but they exist **primarily
for testing and tooling**, not for normal application logic.

⚠️ **Stability Notice**  
These functions are **not part of SDOM’s long-term stable public API**.  
They may be **relocated**, **renamed**, or **removed** in a future major release once
a dedicated testing harness or editor-only extension layer exists.

For now they remain exposed to support:
- Deterministic unit tests  
- Headless CI runs  
- Automated input simulation  
- Editor/inspection overlays  

Normal applications should rely on standard SDL-driven input events.

Public API (External Bindings)
------------------------------

| C++ API                                       | C API Identifier                | Lua Binding                        | Status    | Notes |
|-----------------------------------------------|----------------------------------|--------------------------------------|-----------|-------|
| `void handleTabKeyPress()`                    | `SDOM_HandleTabKeyPress`         | `Core:HandleTabKeyPress()`           | Existing  | Moves focus forward. **Testing/editor only.** |
| `void handleTabKeyPressReverse()`             | `SDOM_HandleTabKeyPressReverse`  | `Core:HandleTabKeyPressReverse()`    | Existing  | Moves focus backward. **Testing/editor only.** |
| `void setKeyboardFocusedObject(DisplayHandle)` | `SDOM_SetKeyboardFocus`        | `Core:SetKeyboardFocus(handle)`      | Existing  | Direct focus override. **Not intended for production use.** |
| `DisplayHandle getKeyboardFocusedObject() const` | `SDOM_GetKeyboardFocus`      | `Core:GetKeyboardFocus()`            | Existing  | Useful for deterministic tests. |
| `void clearKeyboardFocusedObject()`           | `SDOM_ClearKeyboardFocus`        | `Core:ClearKeyboardFocus()`          | Existing  | Clears focus. |
| `void setMouseHoveredObject(DisplayHandle)`   | `SDOM_SetMouseHover`             | `Core:SetMouseHover(handle)`         | Existing  | Synthetic hover assignment. |
| `DisplayHandle getMouseHoveredObject() const` | `SDOM_GetMouseHover`             | `Core:GetMouseHover()`               | Existing  | Query hover state. |
| `void clearMouseHoveredObject()`              | `SDOM_ClearMouseHover`           | `Core:ClearMouseHover()`             | Existing  | Clears hover. |

Intended Usage Notes
--------------------

- **Not recommended** for general application development.  
  SDOM will manage focus/hover automatically from real events.

- **Threading requirement:**  
  Must be called on SDOM’s main thread — modifying focus/hover during traversal
  could cause state inconsistencies.

- **Possible future changes:**  
  A later SDOM version may move these APIs into:
  - a `SDOM_Test` auxiliary module,  
  - a dedicated “Editor” extension block,  
  - or a specialized simulation subsystem.

Rationale
---------

Although niche, these bindings significantly simplify:
- Writing deterministic unit tests  
- Running SDOM headlessly  
- Validating DOM structure in CI environments  
- Implementing external editors or training UIs

Therefore they remain public *for now*, with clear warnings about their
non-stability.

Frame & Loop Diagnostics
------------------------

| C++ API | C API Identifier | Lua Binding | Status | Notes |
| --- | --- | --- | --- | --- |
| `int getFrameCount() const` | `SDOM_GetFrameCount` | `Core:GetFrameCount()` | Existing | Useful for deterministic tests. |

---

Display / Asset Object Creation
-------------------------------

> **Modern SDOM Standard:**  
> SDOM objects are now constructed from **InitStructs** (C++/C API) or **JSON** (file or memory).  
> Lua-table and Lua-script constructors remain *legacy/optional* and only exist when SDOM is built
> with Lua enabled. JSON is the authoritative cross-language initializer.

| C++ API | C API Identifier | Lua Binding | Status | Notes |
| --- | --- | --- | --- | --- |

### Display Creation

| `DisplayHandle createDisplayObject(const std::string& type, const IDisplayObject::InitStruct& init)` | `SDOM_CreateDisplayObject` | `Core:CreateDisplayObjectFromInit(type, init)` | Existing | Primary C/C++ entry point. Uses strongly-typed InitStructs. |

| `DisplayHandle createDisplayObjectFromJson(const std::string& type, const nlohmann::json& j)` | `SDOM_CreateDisplayObjectFromJson` | `Core:CreateDisplayObjectFromJson(type, json)` | Proposed | Calls `<T>::InitStruct::from_json()` then `<T>::CreateFromJson()`. JSON is now the preferred initializer for projects and tooling. |

| *(optional)* `DisplayHandle createDisplayObject(const std::string& type, const sol::table&)` | `SDOM_CreateDisplayObjectFromLuaTable` | `Core:CreateDisplayObject(type, table)` | Legacy / Optional | Only available when Lua is enabled. Deprecated; may be removed in a future release. |

| *(optional)* `DisplayHandle createDisplayObjectFromScript(const std::string&, const std::string&)` | `SDOM_CreateDisplayObjectFromLuaScript` | `Core:CreateDisplayObjectFromScript(type, code)` | Legacy / Optional | Compiles Lua snippet; only exists with Lua enabled. |

---

### Asset Creation

| `AssetHandle createAssetObject(const std::string& type, const IAssetObject::InitStruct& init)` | `SDOM_CreateAssetObject` | `Core:CreateAssetObjectFromInit(type, init)` | Existing | Strongly typed InitStruct constructor. |

| `AssetHandle createAssetObjectFromJson(const std::string& type, const nlohmann::json& j)` | `SDOM_CreateAssetObjectFromJson` | `Core:CreateAssetObjectFromJson(type, json)` | Proposed | Mirrors display-object JSON construction. |

| *(optional)* `AssetHandle createAssetObject(const std::string&, const sol::table&)` | `SDOM_CreateAssetObjectFromLuaTable` | `Core:CreateAssetObject(type, table)` | Legacy / Optional | Lua-only; deprecated. |

| *(optional)* `AssetHandle createAssetObjectFromScript(const std::string&, const std::string&)` | `SDOM_CreateAssetObjectFromLuaScript` | `Core:CreateAssetObjectFromScript(type, code)` | Legacy / Optional | Lua-only; deprecated. |

---

### Notes & Policy

- **JSON is now the canonical cross-language initializer** for both Display and Asset objects.  
  Every SDOM object already implements:
  - `InitStruct` with defaults  
  - `static void InitStruct::from_json(const json&, InitStruct&)`  
  - `static std::unique_ptr<T> CreateFromJson(const json&)`

- The C API should always prefer:
  - `SDOM_CreateDisplayObject(type, initStruct)`
  - `SDOM_CreateDisplayObjectFromJson(type, jsonString)`

- Lua constructors are **optional and deprecated**.  
  They may not appear in future SDOM major releases.

- We should know that **InitStruct + JSON is the modern pipeline**, and table/script constructors are legacy shims.




Object Lookup & Lifetime Management
-----------------------------------

| C++ API | C API Identifier | Lua Binding | Status | Notes |
| --- | --- | --- | --- | --- |
| `IDisplayObject* getDisplayObjectPtr(const std::string& name)` | _n/a_ | `Core:GetDisplayObjectPtr(name)` | Internal | Prefer returning `DisplayHandle` to avoid dangling pointers. |
| `DisplayHandle getDisplayObject(const std::string& name)` | `SDOM_GetDisplayObject` | `Core:GetDisplayObject(name)` | Proposed | — |
| `bool hasDisplayObject(const std::string& name) const` | `SDOM_HasDisplayObject` | `Core:HasDisplayObject(name)` | Proposed | — |
| `IAssetObject* getAssetObjectPtr(const std::string& name)` | _n/a_ | `Core:GetAssetObjectPtr(name)` | Internal | Same pointer safety concern. |
| `AssetHandle getAssetObject(const std::string& name)` | `SDOM_GetAssetObject` | `Core:GetAssetObject(name)` | Proposed | — |
| `bool hasAssetObject(const std::string& name) const` | `SDOM_HasAssetObject` | `Core:HasAssetObject(name)` | Proposed | — |
| `void destroyDisplayObject(const std::string& name)` | `SDOM_DestroyDisplayObject` | `Core:DestroyDisplayObject(name)` | Proposed | Accepts either name or handle (add overload). |
| `void destroyAssetObject(const std::string& name)` | `SDOM_DestroyAssetObject` | `Core:DestroyAssetObject(name)` | Proposed | — |

Orphan & Future Child Management
--------------------------------

| C++ API | C API Identifier | Lua Binding | Status | Notes |
| --- | --- | --- | --- | --- |
| `int countOrphanedDisplayObjects() const` | `SDOM_CountOrphans` | `Core:CountOrphans()` | Proposed | Helps diagnose DOM leaks. |
| `std::vector<DisplayHandle> getOrphanedDisplayObjects()` | `SDOM_GetOrphans` | `Core:GetOrphans()` | Proposed | Returns array of handles. |
| `void detachOrphans()` | `SDOM_DetachOrphans` | `Core:DetachOrphans()` | Proposed | Removes unparented nodes from the stage. |
| `void collectGarbage()` | `SDOM_CollectGarbage` | `Core:CollectGarbage()` | Proposed | Force-destroys unreachable objects (display + assets). |
| `void attachFutureChildren()` | `SDOM_AttachFutureChildren` | `Core:AttachFutureChildren()` | Proposed | Flushes deferred parenting queue. |
| `void addToOrphanList(const DisplayHandle& orphan)` | `SDOM_AddOrphan` | `Core:AddOrphan(handle)` | Proposed | Typically internal but can aid editor tooling. |
| `void addToFutureChildrenList(const DisplayHandle& child, const DisplayHandle& parent, bool useWorld, int worldX, int worldY)` | `SDOM_AddFutureChild` | `Core:AddFutureChild(child, parent, opts)` | Proposed | Provide struct parameter in C for clarity. |

Utility & Introspection
-----------------------

| C++ API | C API Identifier | Lua Binding | Status | Notes |
| --- | --- | --- | --- | --- |
| `std::vector<std::string> getDisplayObjectNames() const` | `SDOM_GetDisplayObjectNames` | `Core:GetDisplayObjectNames()` | Proposed | Returns null-terminated string list in C. |
| `void clearFactory()` | `SDOM_ClearFactory` | `Core:ClearFactory()` | Proposed | Tears down every registered object type. |
| `void printObjectRegistry() const` | `SDOM_PrintObjectRegistry` | `Core:PrintObjectRegistry()` | Proposed | Debug helper; consider piping into error log callback. |
| `std::vector<std::string> getPropertyNamesForType(const std::string& type)` | `SDOM_GetPropertyNamesForType` | `Core:GetPropertyNamesForType(type)` | Proposed | Useful for editor UIs. |
| `std::vector<std::string> getCommandNamesForType(const std::string& type)` | `SDOM_GetCommandNamesForType` | `Core:GetCommandNamesForType(type)` | Proposed | — |
| `std::vector<std::string> getFunctionNamesForType(const std::string& type)` | `SDOM_GetFunctionNamesForType` | `Core:GetFunctionNamesForType(type)` | Proposed | — |

Testing & Input Filtering
-------------------------

| C++ API | C API Identifier | Lua Binding | Status | Notes |
| --- | --- | --- | --- | --- |
| `void setStopAfterUnitTests(bool stop)` | `SDOM_SetStopAfterUnitTests` | `Core:SetStopAfterUnitTests(stop)` | Existing | Already wired through C API + Lua. |
| `bool getStopAfterUnitTests()` | `SDOM_GetStopAfterUnitTests` | `Core:GetStopAfterUnitTests()` | Existing | Complements the setter. |
| `void setIgnoreRealInput(bool)` | `SDOM_SetIgnoreRealInput` | `Core:SetIgnoreRealInput(flag)` | Proposed | Shields automated tests from physical mouse events. |
| `bool getIgnoreRealInput() const` | `SDOM_GetIgnoreRealInput` | `Core:GetIgnoreRealInput()` | Proposed | — |
| `float getKeyfocusGray() const` | `SDOM_GetKeyfocusGray` | `Core:GetKeyfocusGray()` | Proposed | Visualization helper for focus overlays. |
| `void setKeyfocusGray(float)` | `SDOM_SetKeyfocusGray` | `Core:SetKeyfocusGray(value)` | Proposed | — |

Event Helpers
-------------

| C++ API | C API Identifier | Lua Binding | Status | Notes |
| --- | --- | --- | --- | --- |
| `void pumpEventsOnce()` | `SDOM_PumpEventsOnce` | `Core:PumpEventsOnce()` | Proposed | Useful for headless tests that do not run the full loop. |
| `void pushMouseEvent(const sol::object& args)` | `SDOM_PushMouseEvent` | `Core:PushMouseEvent(args)` | Proposed | Accepts Lua tables describing synthetic mouse events. |
| `void pushKeyboardEvent(const sol::object& args)` | `SDOM_PushKeyboardEvent` | `Core:PushKeyboardEvent(args)` | Proposed | — |

Capability Queries
------------------

| C++ API | C API Identifier | Lua Binding | Status | Notes |
| --- | --- | --- | --- | --- |
| `bool hasLuaSupport() const` *(new)* | `SDOM_HasLuaSupport` | `Core:HasLuaSupport()` | Proposed | Returns `true` when the engine was built with Lua enabled **and** the Lua runtime has been initialized. Boolean reports capability instead of pass/fail, so this is an allowed exception to the common signature guideline. |

Version Information
-------------------

| C++ API | C API Identifier | Lua Binding | Status | Notes |
| --- | --- | --- | --- | --- |
| `std::string getVersionString() const` | `SDOM_GetVersionString` | `Core:GetVersionString()` | Proposed | Human-friendly `major.minor.patch`. |
| `std::string getVersionFullString() const` | `SDOM_GetVersionFullString` | `Core:GetVersionFullString()` | Proposed | Includes codename/build metadata. |
| `int getVersionMajor() const` | `SDOM_GetVersionMajor` | `Core:GetVersionMajor()` | Proposed | — |
| `int getVersionMinor() const` | `SDOM_GetVersionMinor` | `Core:GetVersionMinor()` | Proposed | — |
| `int getVersionPatch() const` | `SDOM_GetVersionPatch` | `Core:GetVersionPatch()` | Proposed | — |
| `std::string getVersionCodename() const` | `SDOM_GetVersionCodename` | `Core:GetVersionCodename()` | Proposed | — |
| `std::string getVersionBuild() const` | `SDOM_GetVersionBuild` | `Core:GetVersionBuild()` | Proposed | — |
| `std::string getVersionBuildDate() const` | `SDOM_GetVersionBuildDate` | `Core:GetVersionBuildDate()` | Proposed | — |
| `std::string getVersionCommit() const` | `SDOM_GetVersionCommit` | `Core:GetVersionCommit()` | Proposed | — |
| `std::string getVersionBranch() const` | `SDOM_GetVersionBranch` | `Core:GetVersionBranch()` | Proposed | — |
| `std::string getVersionCompiler() const` | `SDOM_GetVersionCompiler` | `Core:GetVersionCompiler()` | Proposed | — |
| `std::string getVersionPlatform() const` | `SDOM_GetVersionPlatform` | `Core:GetVersionPlatform()` | Proposed | — |

Internal / Private Helpers (Not for C API)
------------------------------------------

These routines remain firmly inside the C++ implementation and should not gain C or Lua surfaces:

- Constructors / destructor (`Core()`, `~Core()`, deleted copy/move operators).
- SDL plumbing helpers: `shutdown_SDL()`, `refreshSDLResources()`.
- Self-test & config deferral: `coreTests_()`, `requestConfigApply()`, `applyPendingConfig()`.
- Binding hook: `registerBindingsImpl(const std::string&)` (protected override).

Open Questions / Follow-ups
---------------------------

1. **Handle exposure vs. raw pointers** – functions currently returning `Stage*`, `IDisplayObject*`, `IAssetObject*`, or SDL pointers should either stay internal or gain safe handle-based wrappers before generating bindings.
2. **Config mutation granularity** – decide whether per-field getters/setters are needed in C, or if manipulating an `SDOM_CoreConfig` blob suffices.
3. **Async safety** – some proposed APIs (`SetIsRunning`, focus setters) mutate state that is only safe on the main thread; generation should embed thread-safety warnings.
