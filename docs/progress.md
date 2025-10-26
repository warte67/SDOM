# Project Description and Development Progress
SDOM is a C++23 library built on SDL3 that brings a DOM‑like scene graph and GUI toolkit to 2D applications and games. The aim is a small, predictable core for layout, events, and rendering so SDL developers can compose interfaces and in‑game UI with familiar patterns and a minimal API surface.

Architecturally, a single Core orchestrates the SDL runtime (window, renderer, main loop). A Stage serves as the root of the display tree, while an EventManager dispatches input through capture → target → bubble phases. The Factory registers, creates, and tracks resources and display objects by type and unique name, returning typed handles for safe references. Layout is driven by an edge‑based anchoring system that supports asymmetric, responsive designs.

Scripting and configuration are first‑class with Lua via Sol2 to enable data‑driven scenes, rapid iteration, and test automation. You can also build everything directly in C++—Lua is optional—but when enabled it can construct objects, wire scenes, drive dialog, and swap stages at runtime without recompilation.

The project emphasizes clear, living documentation. The Markdown design docs are working papers that evolve with the code. Expect them to change as features land and APIs are refined. Public APIs are defined in headers and surfaced via Doxygen, with correctness exercised by unit tests; if a doc briefly diverges from current behavior, treat the code as source‑of‑truth until the docs catch up. Diagrams are authored in Mermaid and exported to static SVG/PNG by the repo’s export pipeline, keeping visuals portable while ensuring the sources remain easy to edit. Progress updates will call out notable documentation changes, and contributions to clarify or correct docs are welcome.

This page is the hub for progress. The “Progress Updates” below record milestones chronologically with context and notes; the “Next Steps” and “ToDo” sections outline near‑term priorities and open tracks. Check back to see what landed and what’s coming next—and feel free to open issues or proposals as the API stabilizes.


## Scripting and Configuration
Lua (via Sol2) is first‑class but optional—you can script scenes and behavior in Lua, build everything in C++, or mix both. The Factory accepts `sol::table` configs alongside initializer structs, and Core’s lifecycle hooks make it easy to integrate scripts for iteration and tests. See the dedicated design doc for details and examples: [Scripting and Configuration (Lua + C++)](scripting_and_configuration.md).


# Progress Updates

## [July 2, 2025]
- Initial rough draft and proof of concept.

---   
## [August 5, 2025] 
- SDOM secondary rough draft

---   
## [September 19, 2025]
- Proof of concept approved. Main development approved. Work begins officially.
- Improved Doxygen documentation for all major headers and macros.
- Added and refined Doxygen comments for SDOM_CLR.hpp, SDOM.hpp, and SDOM_IDataObject.hpp.
- Grouped and documented interface methods in SDOM_IDataObject for clarity and API usability.
- Updated build script to run Doxygen from the correct folder, ensuring documentation builds reliably.
- Fixed Doxyfile issues (footer path, PROJECT_BRIEF syntax) for clean documentation generation.
- Validated Doxygen output for class, group, and member documentation.
- `IDataObject` designed.  This base interface class handles JSON parsing and serialization.
- Core class scaffold implemented as a thread-safe singleton.
- Core initializes SDL, window, renderer, and texture with correct sizing and color from config.
- Window and renderer display with correct logical size and background color.
- Configuration via struct, JSON string, and JSON file is working and uses sensible defaults.
- Main loop, event polling, and error handling are scaffolded and ready for extension.
- Added six callback/hook registration methods to `Core`: `registerOnInit()`, `registerOnQuit()`, `registerOnUpdate()`, `registerOnEvent()`, `registerOnRender()`, `registerOnUnitTest()`.
- Callbacks are invoked at the appropriate lifecycle and event points, enabling user-defined logic for initialization, quitting, updating, event handling, rendering, and unit testing.
- Added `preserveAspectRatio` and `allowTextureResize` flags to `CoreConfig` for flexible window/texture resizing.
- Window resizing now supports two modes: aspect ratio preservation and free resizing, controlled by config.
- Texture resizing can be disabled via config, allowing fixed-resolution rendering regardless of window size.
- JSON configuration supports both new flags for runtime flexibility.
- Refined aspect ratio logic to use a static float for original aspect ratio, ensuring accuracy during repeated resizing.

---   
## [September 20, 2025]
- Factory and resource_ptr scaffolding completed; code compiles and runs cleanly.
- Event system foundation implemented and documented.
- Doxygen documentation added for major interfaces (Core, Factory, IResourceObject).
- Example usage of Core and Factory included in documentation.
- Circular include and pointer management issues resolved (raw pointer for Factory ownership).
- The SDOM framework now has a working Core singleton, Factory resource management, and resource_ptr smart handle. The codebase is stable, maintainable, and ready for expanded testing and feature development.

---   
## [September 21, 2025]
- Renamed `resource_ptr` to `ResourceHandle` throughout the codebase for clarity.
    - The new name better reflects its role as an opaque handle or key for resource lookup, rather than a smart pointer with ownership semantics.
    - Updated all documentation, examples, and comments to use `ResourceHandle`.
- Factory resource management and `ResourceHandle` logic fully tested and confirmed:
    - Resources can be added, retrieved, and removed from the Factory.
    - Multiple `ResourceHandle` instances correctly reflect resource existence and become invalid after removal.
    - Type-safe access via `dynamic_cast` is working and robust.
- Debug output and test scaffolding added to verify resource lifetime and pointer behavior.
- Resolved copy constructor and assignment operator issues for `ResourceHandle`.
- Confirmed that the Factory now contains a working `Stage` resource.
- Next step: Implement parent/child relationships for resources and display objects.
- Unit test framework will be moved to `onUnitTest()` methods once implemented.
- **Resource Creation & Initialization:**
    - Verified that resources (especially Stage) are created via the Factory and properly initialized with onInit().
    - Debug output confirms lifecycle methods (onInit, onQuit, onUpdate, onRender) are called for the root stage.
- **Core & Factory Integration:**
    - Ensured the root node (mainStage) is set and found by Core, with correct debug output.
    - Improved resource creation logic to always use Factory::create, guaranteeing initialization and registration.
- **Recursive Traversal:**
    - Implemented and tested recursive lambdas for shutdown (onQuit) and unit testing (onUnitTest), with correct type handling and null checks.
- **Bug Fixes & Stability:**
    - Diagnosed and resolved segmentation faults by adding null checks and debug output.
    - Tested resource handles and pointer management are more robust and safer, but further testing may be required.
- **Testing & Debugging:**
    - Added debug prints to trace resource creation and lifecycle events.
    - Validated that the system is ready for expanded DOM tree parsing and more complex resource hierarchies.

---   
## [September 22, 2025]
- **Unit Test Framework Refined:**  
    - Simplified the unit test system: now every resource implements `onUnitTests()` for built-in tests.
    - External/custom unit tests are registered and run via the `registerOnUnitTest()` callback, keeping the API minimal and flexible.
    - Removed unnecessary callback registration logic from `IUnitTest`, relying on virtual methods and user callbacks for extensibility.
- **Consistent Naming:**  
    - Standardized all unit test-related identifiers to use the plural form (`Tests`) for clarity and consistency across the codebase.
- **External Test Integration:**  
    - Users no longer need to register callbacks for external unit tests.  
    - Instead, external unit test functions (like `Core_UnitTests()` and `Factory_UnitTests()`) are simply called directly from within the already established `registerOnUnitTest()` callback in `main.cpp`.  
    - This approach makes it straightforward to create and run custom unit tests or build a dedicated test application, while maintaining clear separation between built-in and user-defined tests.
- **API Cleanup:**  
    - Removed redundant or overly complex code from the unit test infrastructure.
    - Ensured the API remains clean, maintainable, and easy to extend.
- **Debugging & Issue Resolution:**  
    - Diagnosed and resolved issues with singleton access, reference binding, and infinite recursion in callback registration.
    - Improved understanding of C++ function pointer and lambda usage for flexible test registration.
- **Expanded Test Coverage:**  
    - Added more unit tests for Factory, Stage, and display objects.
    - Confirmed that the unit test system works well for new features.
    - Identified the need for a custom test object (e.g., `Box`) to facilitate comprehensive anchoring and coordinate propagation tests.
    - Planned to add live event tests (drag/drop, deferred add/remove) and prioritize tests for new features as they are developed.
    - Recognized that coverage for older features will be expanded over time, focusing first on the most critical areas.

---   
### [September 23, 2025]
- **Event System Preliminary Testing:**  
    - Successfully integrated and tested the event system with live SDL3 events.
    - Verified that mouse, keyboard, window, and focus events are received and dispatched correctly.
    - Confirmed that custom display objects (Box) receive and handle events as expected.
    - Minimal "life tests" for event propagation and Box object interaction are passing.
    - Debug output confirms event flow from SDL3 through Core, EventManager, and into display objects.
    - Refactored event dispatch logic for single-stage architecture, simplifying code and improving maintainability.
    - Cleaned up circular includes and pointer conversion issues, improving build stability.
    - Next steps: Expand event tests for Box (drag/drop, stacking, anchoring), add more interactive unit tests, and document event system usage.
    - Refactored codebase to use new `DisplayObject` and `ResHandle` types throughout.
    - Verified and debugged handle logic, ensuring safe pointer access and robust operator overloading.
    - Cleaned up event system: `EventManager` now fully leverages the handle system, removing unnecessary casts and improving safety.
    - Fixed segmentation faults and infinite recursion issues in tree traversal by clarifying handle usage and recursive logic.
    - Improved code readability and maintainability by removing legacy patterns and redundant checks.
    - All major event and display object interactions now use the new handle API.
    - Codebase is stable, with most reference and legacy issues resolved.

---   
## [September 24, 2025]
- **Event System Debugging:**
    - Resolved compile-time issues with `ListenerEntry` constructor and `push_back` calls in `addEventListener`.
    - Verified that `addEventListener` correctly registers event listeners for both capture and bubbling phases.
    - Debugged and confirmed proper event propagation through the DOM hierarchy.
    - Fixed issues with `stopPropagation()` and `setPhase()` methods, ensuring they work as intended.
    - Verified that `stopPropagation()` halts event propagation during the target phase, skipping the bubbling phase.
- **Box Object Testing:**
    - Tested `Box` object interactions with the event system:
        - Verified correct output for capture, target, and bubbling phases.
        - Confirmed that `stopPropagation()` prevents bubbling as expected.
    - Stacked `redBox` on `blueBox` and `greenBox` on `redBox` to test event propagation.
    - Validated event listener behavior for `MouseClick` events in all phases.
        The Box object is not perfect, but it works well enough for testing and serves as a proof of concept for an external application using the SDOM API.
- **Code Cleanup:**
    - Removed debug logs and redundant code from `addEventListener` and `triggerEventListeners`.
    - Simplified event listener registration and dispatch logic.

- **Milestone:** Event System Stability
    - After extensive debugging and testing, the `EventManager`, `EventListeners`, and overall event system are now stable and behave according to typical DOM specifications.
    - Both instant and deferred `addChild()` and `removeChild()` operations work reliably, ensuring robust event propagation and hierarchy management.
    - With the event system confirmed stable, we’ve decided this is the ideal time to begin transitioning from `JSON-based` initialization to `Sol2-powered Lua` scripting.

---   
## [September 25, 2025]
- **JSON → Lua Transition Complete:**  
    - All configuration, scripting, and resource creation now use Lua tables via Sol2.
    - Factory, Core, and EventManager refactored to accept and process `sol::table` configs.
    - All display object constructors updated to support Lua-based initialization.
    - Removed legacy JSON code paths and tests.
- **Unit Tests Passing:**  
    - All core, factory, display object, and stage unit tests now pass with Lua configs.
    - Verified resource creation, retrieval, type checking, and destruction using Lua.
    - Event system and Box object tests run successfully with Lua-driven scenes.
- **Robust Numeric Handling:**  
    - Fixed Sol2 numeric type issues by casting Lua numbers to C++ types as needed.
    - Optional properties (`x`, `y`, `width`, `height`) now default sensibly if missing.
- **Improved Type Assignment:**  
    - All display objects correctly assign and report their type from Lua configs.
    - Resource handles and type checks now work as expected.
- **Documentation and API Consistency:**  
    - Updated documentation to reflect Lua-first configuration and scripting.
    - All examples and tests now use Lua for initialization and setup.
- **Milestone:**  
    - SDOM is now fully Lua-configurable and ready for advanced scripting, rapid prototyping, and integration with external tools. Json features have been removed completely.

---   
## [September 26, 2025]
- **Box Unit Tests Complete:**  
    - Finalized comprehensive unit tests for the Box object, covering creation, hierarchy, anchor logic, and world coordinate propagation.
    - All anchor setters and edge references are tested for all anchor points and hierarchy levels.
    - Confirmed that the Box object works as a reliable proof of concept for external applications using the SDOM API.
    - Decided to defer exhaustive reference calculation tests for resizing and complex anchor scenarios until a specific layout issue is identified.
    - The Box unit test suite is now robust and maintainable; targeted tests will be added as new bugs or features arise.
- **Orphaned Display Object Management:**  
    - Implemented `Factory::countOrphanedDisplayObjects()`, `getOrphanedDisplayObjects()`, and `destroyOrphanedDisplayObjects()` for robust orphan detection and cleanup.
    - Improved `destroyOrphanedDisplayObjects()` to recursively remove all orphaned objects, including nested orphans, ensuring a clean DOM state after parent removal.
    - Added unit tests to verify orphan detection and recursive cleanup logic.
    - Documented the current behavior and discussed future enhancements for selective orphan preservation.
    - Proposed an `OrphanCleanupPolicy` enum for `IDisplayObject` to allow marking objects as persistent, templates, or editor-only, enabling fine-grained control over orphan cleanup (feature deferred for future implementation).
- **General Stability:**  
    - All core, factory, display object, and stage unit tests are passing.
    - Event system and Lua-driven configuration are stable and fully integrated.
    - Documentation and codebase are up-to-date with recent changes.

---   
## [September 27, 2025]
- **Lua Integration Complete:**  
    - All major Core and Factory methods are now exposed to Lua via Sol2 usertype registration.
    - Overloaded methods and custom types (DisplayObject, SDL_Color) are correctly bound and accessible from Lua scripts.
    - The Lua API is stable, with all required commands available for scripting, configuration, and test automation.
- **Lua/C++ Unit Test Integration Enhanced:**  
    - Refined Lua-driven unit tests for display object creation, stage hierarchy, and destruction.
    - Confirmed that DisplayObjects automatically become invalid (`nullptr`) when their resource is destroyed in the Factory, ensuring safe reference semantics.
    - Improved `IDisplayObject::hasChild()` logic to check both handle presence and validity, preventing false positives when objects are deleted.
    - All current unit tests now pass, including tests for creation, addition to stage, and destruction/removal verification.
    - Added Lua-side debugging utilities: Factory registry and stage tree can be printed directly from Lua scripts for live inspection.
    - Demonstrated robust C++/Lua test integration: Lua scripts return boolean results to C++ test framework, enabling seamless cross-language validation.
    - Codebase and Lua API stability increased, with reliable resource management and hierarchy checks.

---   
## [September 28, 2025]
- **Lua Binding Improvements:**
    - Refined Lua usertype registration to ensure idempotent and consistent bindings.
    - Verified that all properties, commands, and functions are correctly exposed to Lua for Core, Factory, DisplayObject, and IDisplayObject.
    - Added missing getters (`getX`, `getY`, `getWidth`, `getHeight`) to DisplayObject Lua bindings.
    - Ensured that overloaded methods are properly handled in Lua.
    - Centralized Lua bindings: Factory now maintains a registry (`registerLuaProperty` / `registerLuaCommand` / `registerLuaFunction`) and applies entries idempotently in a two‑phase process (create minimal sol2 usertype, then attach registry entries).
    - Lua API & binding fixes: `DisplayObject` and `Core` bindings improved (correct boxing/forwarding, colon-call signatures); EventType constants exposed; Sol2 runtime issues (nil-call/userdata mismatches) resolved.
    - Test helpers: added `Core:pushMouseEvent({x,y,type,button})` and `Core:pumpEventsOnce()` so Lua can synthesize and dispatch SDL events deterministically.
    - DisplayObject Lua conveniences: `addEventListener` forwarding and simple getters (`getX`/`getY`/`getWidth`/`getHeight`) exposed to Lua.
    - Box test scaffolding: added simple Box functions (doSomething, resetColor) and a click counter used by synthetic-event tests.
    - Lua-driven tests added/updated:
        - test5: getPropertyNamesForType("blueishBox")
        - test6: getFunctionNamesForType("blueishBox")
        - test7: getCommandNamesForType("blueishBox")
        - test8: C++-synthesized MouseClick → Box listener
        - test9: Lua-only listener + synthetic click (restores keyboard focus to Stage)
    - Cleanup: removed temporary debug instrumentation and test-only Factory helpers.
    - Status: All core C++ unit tests and Lua-driven tests pass locally.
- **Event System Enhancements:**
    - Improved synthetic event handling in Lua with `Core:pushMouseEvent()` and `Core:pumpEventsOnce()`.
    - Verified that synthetic events correctly trigger event listeners on display objects.
- **Box Object Enhancements:**
    - Added utility functions to Box for testing purposes (e.g., `doSomething`, `resetColor`).
    - Implemented a click counter to track interactions during tests.
- **Unit Test Expansion:**  
    - Added new Lua-driven unit tests to cover property, command, and function retrieval for custom display objects.
    - Verified that all existing tests pass successfully with the latest Lua bindings.
- **Code Cleanup:**
    - Removed temporary debug instrumentation and test-only Factory helpers.
    - Ensured the codebase is clean and maintainable.

---   
## [September 29, 2025]
- **Lua Binding Debugging:**
    - Added controlled debug prints to Lua usertype registration to trace binding operations.
    - Verified that each usertype (DisplayObject, IDisplayObject, Stage, Factory, Core) is registered exactly once, preventing conflicts and ensuring consistent behavior.
    - Debug prints can be toggled with the `DEBUG_REGISTER_LUA` constant in `SDOM.hpp`.
- **Binding Consistency:**
    - Ensured that all Lua bindings are idempotent and do not conflict with each other.
    - Verified that overloaded methods and custom types are correctly handled in Lua.
- **Code Cleanup:**
    - Removed unnecessary debug prints from production code, leaving controlled logging for future debugging needs.
    - Ensured the codebase remains clean and maintainable.
- **Core Lua Wrappers Verified:**
    - Completed verification of Core/Factory Lua wrapper coverage (get/set window title, event helpers, DisplayObject forwards, focus/hover helpers, object lookup, and orphan management commands exposed to Lua).
    - Added `test20_lua` which validates `Core:getWindowTitle()` and `Core:setWindowTitle()` round-trip behavior; the `examples/test/prog` test binary builds and runs with the new test.
    - As requested, the `clearFactory` Lua binding was removed (it can be reintroduced later if needed).
- **Lua Bounds / DisplayObject updates (today):**
    - Exposed a typed `Bounds` userdata to Lua (fields: `left/top/right/bottom`, integer/float helpers) and added method-style getters (`getLeft`/`getTop`/`getRight`/`getBottom`/`getX`/`getY`/`getWidth`/`getHeight`) plus an epsilon-aware helper `almostEqual(a,b,eps?)` for robust comparisons.
    - Synchronized `Bounds` registrations so both `DisplayObject` and `IDisplayObject` present a consistent Lua API.
    - Added/updated DisplayObject forwards so Lua can call `h:getBounds()` (returns `Bounds` userdata) and `h:setBounds({...})` (accepts a Lua table), and added edge getters (`getLeft`/`getTop`/`getRight`/`getBottom`) on the handle.
    - Reworked the IDisplayObject unit tests: made `test10` a Lua-only bounds comparison and added `test11` to drive `setBounds` from Lua and verify `getX`/`getY`/`getWidth`/`getHeight` and edge getters.
    - Verified the change by building (`./compile`) and running the test binary (`./prog`); all unit tests passed locally.
- **Lua wrapper headers verified:**
    - Both primary bulk Lua wrapper headers, `lua_Core.hpp` and `lua_IDisplayObject.hpp`, have complete implementations and are exercised by the `examples/test` Lua unit tests. The test run (`./compile clean && ./prog`) confirms Lua-level coverage for creation, property access, event helpers, DisplayObject forwards, anchors/edges, geometry, and factory helpers.
    - Temporary debug prints used during binding debugging were cleaned up; controlled registration logging remains toggleable via `DEBUG_REGISTER_LUA` in `SDOM.hpp`.
- **DisplayObject_UnitTests:**
    DisplayObject_UnitTests now contains:
    - A concise numeric mapping header above the DisplayObject tests.
    - Full human-readable descriptions (matching the strings passed to UnitTests::run) in place of the short mapping comments.
    - Test functions renamed to DisplayObject_test1 … DisplayObject_test10.
    - The tests vector updated to call the new functions and includes matching descriptive comments.
    - Debug output updated to reference DisplayObject_test10 where applicable.
    - A clean build and test run were executed successfully.    
    - Reworked the DisplayObject unit tests for readability and maintainability: added a numeric mapping header, expanded inline descriptions to match `UnitTests::run` strings, and renamed test functions to `DisplayObject_test1`..`DisplayObject_test10`.         

---   
## [September 30, 2025]
- **Summary:**
    - Consolidated work focused on lifecycle event dispatch, Lua bindings robustness, and logging/debug cleanup.  These changes make lifecycle events available to Lua listeners, surface Lua errors, and stabilize the debug/logging surface for continued iteration.
- **Event dispatch & lifecycle hooks:**
    - Factory now emits EventType::OnInit immediately after object initialization (useful for global/stage listeners).
    - Core dispatches EventType::OnQuit globally during shutdown via EventManager::dispatchEvent().
    - Core::run() now dispatches stage-level listener events:
        - EventType::OnEvent — after queued SDL event dispatch
        - EventType::OnUpdate — immediately before onUpdate traversal
        - EventType::OnRender — immediately after onRender, before present
    - For Update/Render/Event hooks we dispatch to event-listeners only (dispatchEventToAllEventListenersOnStage) to avoid duplicating node->onEvent() behavior.
    - Note: these dispatch points are listeners hooks; they do not replace per-node onUpdate()/onRender() unless an API is added to allow listeners to disable default behavior.
- **Lua: Event / EventType bindings & robustness:**
    - Centralized EventType → Lua exposure using the EventType registry to avoid string mismatches.
    - Added a Lua usertype for Event exposing: dt (elapsed), type/typeName, target (DisplayObject) and convenience accessors name / getName() (prefers target name, falls back to event type).
    - Made lua listener registration use table form { type = EventType.X, listener = fn } in examples.
    - Replaced inline lambdas in examples with named callbacks in examples/test/lua/callbacks/listener_callbacks.lua.
    - Wrapped all Lua callback invocations with sol::protected_function to log Lua errors (no silent failures).
    - Fixed multiple example Lua issues (missing 'end' in render.lua, undefined EMA vars) so OnUpdate/OnEvent/OnRender prints now appear as expected.
- **Event Types & Listener Semantics:**
    - New lifecycle/event hooks added for fine‑grained listener control:
    - OnPreRender — listener‑only, per‑node. Fired immediately after a node renders itself and before its children are rendered (useful for backgrounds or setup that should appear under children).
    - OnRender — listener‑only, per‑node. Fired after a node and all its children have been rendered and before present (useful for overlays, HUDs, screenshots).
    - OnUpdate — per‑node. Listeners run first for the node, then the node's default onUpdate runs (listeners may call disableDefaultBehavior() to prevent the default).
    - OnEvent — input/event dispatch using capture → target → bubble phases; listeners run before the default onEvent at the target and may stopPropagation()/disableDefaultBehavior().
    - OnInit / OnQuit — lifecycle hooks: OnInit is emitted after object creation/initialization, OnQuit is dispatched globally during shutdown.
- **Semantics and best practices:**
    - Listener‑only hooks (OnPreRender/OnRender) are dispatched using the event‑listener path so they do not duplicate node->onEvent() behavior.
    - Listeners receive the same mutable Event instance, so calling stopPropagation() or disableDefaultBehavior() affects subsequent dispatch and default handlers.
    - Per‑node events are targeted at the node's DisplayObject so node‑attached listeners see a meaningful currentTarget.
    - Lua examples use the table form `{ type = EventType.X, listener = fn }` and can rely on `evt.name` / `evt:getName()` to get a sensible name (target name or type).
- **Performance notes:**
    - Hot paths check for listeners before constructing per‑frame Event objects (hasListeners fast‑path) to avoid unnecessary allocations when no listeners are registered.
    - Cache Lua wrapper objects at registration time (sol::protected_function) to avoid per‑dispatch wrapper construction in high‑frequency events such as OnUpdate and mouse motion.
- **Debugging & logging:**
    - Replaced ad-hoc std::cout debug prints with DEBUG_LOG macro and made macro stream-friendly to fix compile-time operator<< usage.
    - Removed or gated noisy temporary debug prints (e.g., pushMouseEvent_lua) and made them controllable so normal runs are not spammy.
    - Left a clear, short comment about EventType::registerAll() retained as a static-init-order safety hedge (user EventTypes still register themselves in their constructors).
- **Build & verification:**
    - Fixed compile regressions introduced during iteration; rebuilt examples/test successfully.
    - Ran examples/test/prog: unit tests pass; observed expected runtime output for OnInit/OnUpdate/OnRender/OnEvent/OnQuit Lua handlers.
    
---
## [October 2, 2025]
- **Synthetic Event Handling Robustness:**
    - Refined synthetic mouse event generation and dispatch to work reliably in both floating and tiled window manager modes.
    - Ensured all synthetic events use `SDL_RenderCoordinatesToWindow()` for correct mapping from logical to window coordinates, fixing previous issues with negative or out-of-bounds coordinates.
    - Updated Lua and C++ event helpers to guarantee robust event delivery and handler invocation.
    - Verified that all mouse-related unit tests (including Lua-driven synthetic click and hover tests) now pass in both environments.
    - Improved event queue draining in `Core::pumpEventsOnce()` to ensure all queued events are dispatched before returning, eliminating timing issues in Lua event handler tests.
    - Added diagnostic logging to synthetic event helpers and event handlers for easier debugging and verification of coordinate mapping and event flow.
- **Unit Test Reliability:**
    - Addressed timing issues in Lua unit tests by polling for event handler completion, ensuring tests do not return before handlers are invoked.
    - Confirmed that both Lua and C++ unit tests pass consistently across different compositors and window manager modes.
    - Documented best practices for synthetic event testing and event queue management.
- **Documentation & Debugging:**
    - Updated documentation to reflect recent changes in event dispatch, Lua bindings, and synthetic event handling.
    - Added notes and troubleshooting tips for running tests under different window managers (floating vs. tiled).
    - Improved error reporting and debug output for configuration loading and Lua script execution.[October 1, 2025]**
    - Lua/Event binding & SDL event exposure (today):
        - Expanded `SDL_Utils::eventToLuaTable()` to expose many SDL3 event fields to Lua (mouse, keyboard, gamepad, pen, touch, camera, audio, sensor, render, user, etc.).
        - Centralized `Event` Lua usertype creation in `SDOM::Event::registerLua()` and added bindings for the full set of Event accessors and mutators so Lua can call methods like `evt:getMouseX()` and use properties `evt.mouseX`.
        - Added `Event::getName()` (C++ implementation) and switched the Lua `name` property to use the member-pointer binding (single source of truth + proper locking).
        - Implemented safe `getPayload`/`setPayload` Lua bindings (payload returned as a Lua table via the current Lua state to avoid lifetime issues).
        - Exposed lower-camelcase convenience properties (e.g., `mouse_x`, `mouse_y`, `wheel_x`, `click_count`, `click_count`, `ascii_code`, etc) as `sol::property` pairs for readable/writable access from Lua.
        - Verified the project builds cleanly after these changes (`./compile` run in `examples/test` succeeded and unit tests built/run as part of the test binary).
    - Notes:
        - Where getters return Lua-managed objects (like `sol::table` payloads) we kept lambda bindings so we can safely create `sol::object` tied to the current `sol::state`.
        - `SDL_Utils` still exposes `eventToLuaTable` as a Lua helper table (`SDL`/`SDL_Utils`) for direct use; `Event::registerLua()` uses the C++ helper internally for the `sdl` property.
        - Example update: `examples/test/lua/callbacks/listener_callbacks.lua` was adjusted to colorize the "target:" label using the DisplayObject `getColor()` Lua binding and `CLR.fg_rgb(r,g,b)`; the code uses `pcall` fallbacks so it degrades gracefully when color or handle info is unavailable.
        - Verified: rebuilt the examples and test binary after these changes (`./compile` in `examples/test`) — build completed and `prog` produced successfully.
        
---   
## [October 3, 2025]
- Cleaned up noisy debug output across the codebase:
    - Replaced ad-hoc std::cout dbg prints with DEBUG_LOG and gated test‑only logs behind a new DEBUG_LUA_TESTS / LUA_INFO wrapper.
    - Converted recent INFO() diagnostics used for Lua testing to LUA_INFO so they are off by default.
- Stabilized and hardened Lua bindings and factory behavior:
    - Exposed Bounds as a proper sol2 usertype so Lua can read/write edges and call width()/height().
    - Ensured SDL_Color and other helper types are Lua‑friendly.
    - Enforced unique display object names in Factory (creation now throws on duplicates); updated unit tests to catch the exception.
- Fixed event / synthetic event issues and test timing:
    - Pump loop improved to drain the event queue completely before returning, removing timing races in Lua tests.
    - Synthetic mouse events now use SDL_RenderCoordinatesToWindow and hit‑testing was verified; added targeted instrumentation to confirm coordinate mapping and top‑hit selection.
    - Mouse hover unit test (Lua #19) diagnostic instrumentation added and the failing hover test was fixed.
- Display object / hierarchy fixes:
    - Prevented duplicate children by name during attachment and added dedupe in sortChildrenByPriority (keeps most‑recent entry).
    - Improved setParent bookkeeping (remove old entries, preserve world bounds, add to new parent), and reduced duplicate insertion regressions.
    - Added Lua overloads for priority/z‑order helpers (accept numeric and table descriptor forms) to match test call shapes.
- Identifier updates (bindings & Lua surface):
    - Performed a sweep of function identifier changes to simplify the Lua/C++ surface and improve consistency:
        - Renamed (old → new):
            - getDisplayObjectHandle → getDisplayObject
            - getStageHandle_lua → getStage_lua
            - getRootNode_lua → getRoot_lua
            - setRootNodeByHandle_lua → setRootNode_lua
            - setStage_lua → setStageByName_lua
            - handleTabKeyPress_lua → doTabKeyPressForward_lua
            - handleTabKeyPressReverse_lua → doTabKeyPressReverse_lua
        - Added:
            - setStage_lua(const DisplayObject& handle) — set stage by handle (Lua helper)
            - setRootNode_lua(const DisplayObject& handle) — helper for root node by handle
            - permissive Lua overloads for addEventListener / removeEventListener (table descriptor + typed args)
        - Removed / consolidated:
            - Legacy getDisplayObjectHandle Lua mapping (removed in favor of getDisplayObject)
            - getRootNodePtr_lua and other raw-pointer helpers (no raw pointer exposure to Lua)
    - Compatibility & notes:
        - Where practical, compatibility shims were left or added briefly (inline forwards) to prevent immediate breakage; these can be removed once downstream scripts are updated.
        - Global `Core` is now consistently exposed as a forwarding table (CoreForward) for predictable colon/dot call semantics; Core userdata remains registered but is not the global to avoid dispatch ambiguity.
- Build & CI fixes:
    - Restored minimal Factory/Core forwarding stubs for deprecated introspection APIs to resolve linker errors during refactors.
    - Rebuilt and validated examples/test/prog — full unit test suite passes on local runs after fixes.
- Developer ergonomics:
    - Added DEBUG_LUA_TESTS macro and CMake toggle guidance so Lua test diagnostics can be enabled without editing source.
    - Left controlled registration logging (DEBUG_REGISTER_LUA) for binding diagnostics.
- Status: ~90% working — core orphan detection and policy handling are implemented and exercised by the unit tests, but a couple of issues remain under investigation.
- Implemented:
    - OrphanRetentionPolicy on IDisplayObject (Manual / AutoDestroy / GracePeriod).
    - Factory orphan queue, getOrphanedDisplayObjects(), and a collectGarbage() pass that respects policies:
        - AutoDestroy → eligible for immediate destruction (when safe).
        - GracePeriod → retained until grace timeout; reparenting during the window cancels destruction.
        - RetainUntilManual → never auto-destroyed.
    - Lua bindings and string helpers to set/get orphan policies from scripts.

---   
## [October 4, 2025]
- Quick fixes & Lua binding polish (morning)
    - Fixed a typo in examples/test Lua callback (evt.taget → evt.target) that caused nil-index errors in right‑click handlers.
    - Added a safe convenience: `IDisplayObject::removeFromParent()` and Lua bindings so scripts can call `evt.target:removeFromParent()` directly from listeners.
    - Implemented recursive removal helpers: `removeDescendant(...)` (depth‑first, first match) and exposed them to Lua on both `IDisplayObject` and `DisplayObject` handle usertypes.
    - Added `removeChild(const std::string&)` overload and corresponding Lua binding so `parent:removeChild("name")` works without userdata coercion; fixed overload registration to avoid Lua/C++ type mismatch.
    - Removed confusing const predicate overloads that were misnamed (they behaved like hasChild checks); cleaned up their Lua wrappers to avoid ambiguity.
    - Added a small compatibility helper `getStage()` in Lua bindings to avoid breaking existing example scripts.
    - Fixed binding registration issues (explicit function-pointer casts / sol::overload) so the new helpers expose unambiguous APIs to Lua.
    - Rebuilt and ran the examples/test suite — compilation succeeded and unit tests passed locally after the changes.
- Next actions
    - Add unit tests that specifically exercise `removeFromParent()`, `removeDescendant()` (handle/name), and `removeChild(name)` semantics (edge cases, nested removals, event-time removals).
    - Consider removing temporary compatibility shims (getStage global, any legacy forwards) once examples are migrated to canonical Core/Factory bindings.
    - Improve docs/comments in headers to clarify difference between strict `removeChild` (direct child) vs. recursive removal and the convenience `removeFromParent()` behavior.   

---   
## [October 5, 2025]
- Refactored Lua Core bindings:
    - Centralized and simplified Core::_registerDisplayObject() by introducing small per-signature binder lambdas (`bind_noarg`, `bind_table`, `bind_string`, `bind_bool_arg`, `bind_do_arg`, `bind_return_*` and `bind_callback_*`).
    - All Core APIs are now registered consistently in three places: usertype methods (colon-call), CoreForward table entries, and optional global aliases — reducing duplication and eliminating subtle behavioral mismatches.
    - Consolidated all registerOn* callback wrappers with per-signature factories so Lua->C++ callback wiring and error handling is uniform.
    - Added permissive handlers for mixed-argument helpers (e.g., setRootNode / setStage accept string, handle, or name-table), and convenient aliases (setRoot -> setRootNode).
    - Improved maintainability: adding new Core functions or aliases requires a single-line registration using the appropriate binder.
    - Build and unit tests pass locally after the changes.
- Notes / next steps:
    - Plan to move the binder lambdas into a dedicated implementation (e.g., src/lua_Core_bindings.cpp) and expose small static helpers so other modules (IDisplayObject, Event, EventType, SDL) can reuse the same registration patterns. Postpone that move until Lua smoke tests and CI are stable.
- Additional Oct 5 updates (implementation details):
    - Added a reusable header and implementation for the bind helpers: `include/SDOM/lua_BindHelpers.hpp` and `src/lua_BindHelpers.cpp`. These centralize the per-signature helper implementations previously expressed as local lambdas.
    - Rewired several Core bindings in `src/SDOM_Core.cpp` to use the new helpers (examples: `quit`, `registerOn`, `setStage`, `setRootNode`, `setRoot`). More inline lambdas remain and will be swept in a follow-up.
    - Replaced a previously-silent catch around `EventType` registration with visible debug logging (`DEBUG_LOG(...)`) and added a short explanatory comment about lifetime/static-init-order assumptions.
    - Verified the change locally: ran the project's compile/test flow (`./compile` in `examples/test`), the library and test binary built and installed headers under `~/.local/include/SDOM` and `~/.local/lib/libSDOM.a` updated.
    - Next short steps: complete a full sweep of remaining inline binding lambdas in `SDOM_Core.cpp`, add a small Lua smoke test exercising name-or-handle binds, and then consider moving the helpers into a dedicated `lua_Core_bindings.cpp` once CI is stable.
        
---   
## [October 6, 2025]
- Lua binding refactor (IDisplayObject):
    - Replaced repetitive sol2 wrappers with small, signature-based helper lambdas using a consistent naming scheme: `bind_{return}_{args}` (e.g., `bind_R_0`, `bind_void_i`, `bind_R_do`, `bind_void_str`, `_nc` variants for non-const).
    - Applied helpers across hierarchy, priority/z-order, focus/interactivity, tab management, geometry/layout, anchors, edge positions, and orphan retention bindings.
    - Kept `sol::overload` only where flexible call shapes are needed (e.g., `setPriority`, `setZOrder`, `moveToTop`, `setToHighest/LowestPriority`).
- Lua wrappers (lua_IDisplayObject.cpp):
    - Added flexible wrappers that accept descriptor tables and handle-or-name targets:
        - `setPriority_lua_any`, `setPriority_lua_target`, `setZOrder_lua_any`, `moveToTop_lua_any`, `setToHighestPriority_lua_any`, `setToLowestPriority_lua_any`.
    - Added a handle-aware `getName_handle_lua` and updated `getName` binding to prefer the live object and fall back to handle name to fix a unit test edge case.
    - Aligned orphan retention helpers (`orphanPolicyFromString`, `orphanPolicyToString`, `setOrphanRetentionPolicy`, `getOrphanRetentionPolicyString`) to helper binders, including an enum binder (`bind_R_orp`).
- Debug/log cleanup:
    - Removed remaining temporary diagnostics (`[DIAG] ...`, `[debug-registerOnLua] ...`) from Lua bindings; normal output is now clean by default.
- Tests:
    - Rebuilt examples/test; unit tests pass after the binding/wrapper updates.
- Maintainability:
    - Binding surface is now uniform and easier to extend; helper pattern reduces boilerplate and mismatches between similar APIs.
    - Next: consider moving common bind helpers into the shared `lua_BindHelpers` so other modules can reuse them, and add brief docs for descriptor-table forms (priority/z-order/moveToTop) in the Lua scripting guide.
- Orphan / garbage-collection work
    - Exposed orphan-grace accessors to Lua: getOrphanGrace / setOrphanGrace (milliseconds).
    - Implemented Lua-side GC test (GarbageCollection_test4):
        - Test temporarily lowers an object's orphan grace to 500ms, removes it, polls Core:collectGarbage(), and verifies the orphan is destroyed within ORPHAN_GRACE_PERIOD + 500ms.
        - Test restored the original grace period on completion.
    - Tests updated to time out based on the C++ ORPHAN_GRACE_PERIOD constant for deterministic behavior.
- Orphan retention / garbage collection implemented:
    - Added OrphanRetentionPolicy on IDisplayObject (AutoDestroy, GracePeriod, RetainUntilManual).
    - Added orphanGrace (ms) and get/set accessors; exposed to Lua (`getOrphanGrace` / `setOrphanGrace`).
    - Factory now tracks orphanedAt and respects policies in collectGarbage().
    - Added Lua bindings and unit tests (GarbageCollection_test2..test4) verifying auto-destroy, grace-period behavior, and manual retention.
    - Tests use ORPHAN_GRACE_PERIOD for deterministic timeouts; SDL.Delay bound for Lua tests.
- SDL / test utilities
    - Bound SDL.Delay to Lua (supports both SDL.Delay(ms) and SDL:Delay(ms) method-call forms) to allow deterministic sleeps from Lua tests.
    - Added small-step wait loop in tests that repeatedly calls Core:collectGarbage() + SDL:Delay() for responsive detection.
- Completed and consolidated Lua‑driven unit tests for DisplayObject / IDisplayObject:
    - Added/validated tests covering dirty/state management, hierarchy (getChild/getParent), focus & interactivity, geometry & layout, priority/z‑order, anchors/edges, and bounds/color handling (tests #11–#15).
    - Anchor tests exercise all 9 AnchorPoint values on each edge and assert that X/Y/Width/Height remain unchanged after each anchor change.
- Lua binding fixes and coverage:
    - Added missing Lua binding for getChild on DisplayObject handles.
    - Fixed logic bug in isClickable test and improved raw‑string safety for embedded Lua chunks.
    - Annotated include/SDOM/lua_IDisplayObject.hpp to reflect which wrappers are exercised by unit tests (TESTED / UNTESTED).
    - Where Lua-to-C++ type mismatches appeared (anchor setters expecting enum), updated Lua test code to map human-friendly strings to enum indices; considered adding C++ overloads for string names as a follow-up.
- Orphan / GC & test utilities:
    - Orphan retention and orphan_grace accessors are exposed to Lua and exercised by garbage‑collection tests; tests temporarily adjust orphanGrace (500ms) for deterministic verification and restore original value after the run.
    - Bound SDL.Delay to Lua and added small-step wait loops (Core:collectGarbage + SDL:Delay) used by GC tests for responsive detection.
- Stability & validation:
    - Rebuilt examples/test and ran the full test suite; all DisplayObject tests pass locally.
    - Cleaned up temporary diagnostic prints; kept controlled debug gating for registration and Lua tests.
- **Performance Milestone:**  
    - The SDOM main loop is now 98% complete and matches the design for the finished library.
    - Benchmarks show well over 10,000 frames per second on development hardware. Roughly 6000-7000 fps on a 15 year old I3 system (roughly 1.5 miliseconds per frame of overhead).
    - The SDOM API introduces only ~100 microseconds of overhead per frame, which is negligible for real-time 2D applications and 2D/3D games. This performance is without extensive post development optimization.
    - This validates the architectural choices and confirms that the system is ready for further feature development and integration.
- **Milestone - DisplayObject Complete:**  
    - With the DisplayObject (IDisplayObject) system now robust, unit-tested, and stable, the next major development track is the AssetObject system for resource management.   
            
---   
## [October 7, 2025]
- cleaned up the heavy includes from the header files to reduce compile times and improve modularity.
- Moved several implementation details from headers to source files, including private methods and member variables.
- Reduced header dependencies by using forward declarations where possible.
- Verified that all unit tests pass after the refactor.
- Improved compile times by approximately 20% in the examples/test project.
- `SpriteSheet` refactor and Lua API changes:
    - Added helpers to resolve/ensure the underlying SDL_Texture (resolve_texture / ensure_texture_loaded).
    - Reordered `drawSprite` APIs so spriteIndex is the first parameter for all C++ overloads and Lua wrappers.
    - Lua `drawSprite` wrappers accept (spriteIndex, x, y) or (spriteIndex, dstTable) and the extended variant expects spriteIndex immediately after the object (method call: ss:drawSprite_ext(idx, src, dst)).
    - Fixed all example call sites (`Box` and listener callbacks) to the new signature and dispatch patterns.
- Lua bindings and tests:
    - Cleaned up Lua registration: removed onLoad/onUnload aliases from Lua handle surface; kept load/unload semantics.
    - Resolved Sol2 numeric/argument ordering issues in wrappers (fixed colon vs dot call mistakes).
    - Examples/test Lua callbacks updated and verified; drawSprite works in the onRender path.
- Build & workflow notes:
    - Prefer running builds from examples/test:
        - ./compile clean  (full rebuild from examples/test)
        - ./compile        (incremental)
        - ./prog           (run test binary)
    - Avoid requiring repository-root ./rebuild under other user permissions.

---   
## [October 8, 2025] 
- drawSprite Lua API / overload consolidation
    - Consolidated Lua drawSprite wrappers to a single exposed name with deterministic dispatch:
        - Exposed variants (C++):
            - `drawSprite(index, x, y, color?, scaleMode?)`          — fast (x,y) draw
            - `drawSprite(index, dstRectF, color?, scaleMode?)`       — destination-rect draw
            - `drawSprite(index, srcRectF, dstRectF, color?, scaleMode?)` — source->dest subregion draw
            - `scaleMode` may be string `"linear"` or `"nearest"` but `"nearest"` is default if omitted.
        - Lua surface:
            - Kept three explicit helpers for clarity: `drawSprite` (x,y form), `drawSprite_dst`, `drawSprite_ext`
            - Also provide a single `drawSprite(...)` dispatcher that inspects argument types/tables and routes to the correct internal overload (heuristic rules documented below).
- Dispatcher rules (short):
    - First argument after the handle must be spriteIndex (number).
    - (index, number, number, ...) → x,y form.
    - (index, table, table, ...) → srcTbl + dstTbl → drawSprite_ext.
    - (index, table, ...) → dstTbl → drawSprite_dst (if ambiguous, src+dst is preferred when two tables are provided).
    - Optional color table accepted as {r,g,b,a} or {r=..,g=..,b=..,a=..}; scaleMode accepted as string or numeric enum.
    - Optional rectangle tables accepted as {x=..,y=..,w=..,h=..} or {left=..,top=..,right=..,bottom=..}.
    - Callers may use explicit `drawSprite_dst` / `drawSprite_ext` to avoid ambiguity.
- Linear-filter "clipping" mitigation:
    - When using "linear" (SDL_SCALEMODE_LINEAR) the renderer now insets source rectangles by ~0.5 texels to reduce neighbor-bleed.
    - Recommended long-term fixes: export atlas with duplicated-edge 1px padding or build a runtime padded atlas with edge replication (sketches available in the codebase).
- Docs & examples updated:
    - Example call sites in examples/test/lua/callbacks/listener_callbacks.lua updated to use explicit dst/ext forms or pass nil placeholders where appropriate.
    - Add unit tests for dispatcher patterns to prevent regressions.
- Label: wrapping, resource resolution, and rendering fixes
    - Fixed Label word-wrapping bug: maxWidth computation now uses parent-relative coordinates so inset labels compute correct available width (previously produced one-word-per-line).
    - Standardized resourceName usage: Lua configs and InitStructs now reference the texture/spritesheet filename (e.g., "default_bmp_8x8"). Label defaults and example configs updated accordingly.
    - Relaxed Label constructor validation: accept SpriteSheet as deferred font source and perform strict IFontObject validation/loading in onInit(), avoiding premature hard-fails during config parsing.
    - Added guarded diagnostics for Label render/tokenization to aid debugging; diagnostics are temporary and gated for cleanup.
    - Verified fixes with the examples/test suite; Label rendering and wrapping now behave correctly in the blueishBox example.

---   
## [October 9, 2025] 
- Codebase rename: 
    - DisplayObject -> DisplayHandle and AssetObject -> AssetHandle — completed across the C++ sources and Lua scripts.
        - Renamed `DisplayObject` to `DisplayHandle` to help separate from the `IDisplayObject` and to imply `Handle` instead of `Object`.
        - Remamed `AssetObject` to `AssetHandle` to help separate fro the `IAssetObject` and to help imply `Handle` rather than `Object`.
    - Lua bindings: 
        - per-instance and prototype handle bindings are registered; unit tests updated to use the new names.
    - Remaining documentation work: 
        - diagrams, design charts and README references still use the old names and must be updated.
- **Initialization & Property Audit:**
    - Audited all constructors (`InitStruct` and `sol::table`) for `IDisplayObject` and related types to ensure every property is initialized with sensible defaults.
    - Verified that all properties are exposed to Lua via `DisplayHandle` using the new identifier conventions (`snake_case` for properties, `lowerCamelCase` for functions).
    - Improved inheritance in Lua: derived types now inherit all properties and functions from `IDisplayObject`.
    - Updated Lua bindings to consistently use `snake_case` for properties and `lowerCamelCase` for functions.
    - Added and documented missing properties in constructors with clear comment blocks for maintainability.
    - Added helper lambdas and backward compatibility shims to ease migration to new naming conventions.
    - Updated documentation and README to clarify property/function naming conventions and inheritance expectations.
    - All changes verified by new Lua test scripts and unit tests; initialization bugs and missing bindings resolved.
    - All properties are now initialized in both constructors (`InitStruct` and `sol::table`).
    - Every property is exposed to Lua via `DisplayHandle` using the new identifier conventions.
    - Properties use `snake_case` and functions use `lowerCamelCase` in both C++ and Lua.
    - Inheritance is verified: derived types in Lua inherit all properties and functions from `IDisplayObject`.
    - Audit of constructors completed; missing properties added and documented with comment blocks.
    - All Lua bindings audited and updated; properties and functions bound with correct naming conventions.
    - Lua test scripts confirm inheritance and property/function access.
    - Documentation and code comments updated to reflect conventions and inheritance expectations.
    - Refactor for consistency completed; helper lambdas and backward compatibility shims added as needed.
- **Startup Refactor**
    - SDOM’s startup logic has been refactored from legacy, implicit global table parsing (e.g., `Core` or `config`) to an explicit initialization pattern. Now, users must directly call `Core:create(config)` and `Core:run()` in both Lua and C++, separating configuration from resource creation and engine startup. This change improves clarity, flexibility, and maintainability, and aligns SDOM with modern engine integration practices. Legacy support for global tables remains during the transition, but new documentation and examples use the explicit method calls.
- **Key changes:**
    - Deprecate automatic parsing of global tables for startup.
    - Require explicit calls to `create` and `run` methods.
    - Separate config parsing from resource creation.
    - Bind engine functions as Lua globals for direct access.
    - Update all documentation and examples to reflect the new pattern.
- **For full details and migration steps, see:**  [docs/startup_refactor.md](../docs/startup_refactor.md)

---   
## [October 11, 2025]
- **BitmapFont & TruetypeFont Resource Creation Improvements:**
    - Refactored resource creation so both BitmapFont and TruetypeFont assets can be registered via the config table or manual `createAsset()` calls.
    - Added automatic TruetypeFont asset creation: a single `createAsset("TruetypeFont", { ... })` call now creates the backing TTFAsset internally if needed, simplifying Lua and C++ workflows.
    - Normalized resource type strings and property names (e.g., `"TruetypeFont"` and `"truetype"` both accepted; `font_size`/`fontSize`/`size` all mapped).
    - Improved BitmapFont creation: SpriteSheet and Texture dependencies are resolved automatically, and renderer initialization order is guaranteed.
    - Verified that config-driven and manual asset creation paths now work identically for all supported font types.
- **Debug & Logging Cleanup:**
    - Removed noisy debug prints and gated diagnostics behind `DEBUG_LOG` and `DEBUG_LUA_TESTS` macros.
    - Silenced duplicate-config and asset re-registration messages for clean production output.
    - Added a config flag to enable resource creation diagnostics for troubleshooting.
    - Rebuilt and validated the codebase after cleanup; all tests pass.
- **Label Rendering & Font Scaling Fixes:**
    - Cleaned up label rendering logic to properly handle resizing, bounds, and culling.
    - Improved word wrapping and text alignment for both BitmapFont and TruetypeFont labels.
    - Verified per-axis bitmap font scaling and Truetype font size propagation.
    - Updated unit tests and example configs to confirm correct rendering and resource usage.
- **TruetypeFont Implementation:**
    - Hardened TTFAsset and TruetypeFont loading, including SDL_ttf initialization and error handling.
    - Factory now exposes a `default_ttf` font and falls back to bitmap fonts if Truetype creation fails.
    - Fixed asset registration and loading bugs; all assets are now tracked and validated by the Factory.
    - Added integration points for future unit tests covering TTFAsset and TruetypeFont.
- **Label Render-to-Texture Optimization:**
    - Refactored Label to render text to a cached texture when dirty, dramatically improving performance.
    - Labels now only re-rasterize text when text, font, style, or bounds change; otherwise, the cached texture is reused for fast redraws.
    - Average frame rate increased from ~100 fps to over 4000 fps in typical scenes (on this 15 year old machine).
    - Even under stress (continually forcing retokenization and redraws), frame rates remain high (~2000 fps), confirming the effectiveness of the optimization.
    - This change enables smooth rendering of large text blocks and many Labels without CPU/GPU bottlenecks.
    - Debug logic for texture rebuilds and alignment is now gated and can be toggled for future troubleshooting.
    - All unit tests and example scenes validated after the optimization.
    
---
## [October 12, 2025]
- **Internal Resource Renaming:**
    - Renamed all default internal resources for clarity and consistency:
        - `default_bmp_8x8` → `internal_font_8x8`
        - `default_bmp_8x12` → `internal_font_8x12`
        - `default_icon_8x8` → `internal_icon_8x8`
        - `default_ttf` → `internal_ttf`
    - Updated all config files, asset registration, and code references to use the new names.
    - This change makes it clear which resources are engine-provided and improves naming consistency across bitmap and truetype fonts.
- **BitmapFont Sizing Fixes:**
    - BitmapFont assets now correctly default to their backing SpriteSheet's sprite size when `font_width` or `font_height` are not specified.
    - Both internal and external bitmap fonts render at their intended glyph dimensions (e.g., 8x12 for `internal_font_8x12`).
    - Added runtime diagnostics to confirm font metrics during asset loading.
- **Label Rendering Improvements:**
    - Label logic now always renders at least one line or token, even when the available height or width is too small to fit the full content.
    - Prevents labels from disappearing when their bounds are smaller than the text, ensuring clipped text is still visible.
    - Verified that Labels handle extreme resizing gracefully, maintaining visibility of partial text.
- **Tab Key Navigation Fixes:**
    - keyboard [TAB] and [SHIFT_TAB] were hitting on labels. 
    - The `tab_enabled` and `tab_priority` properties are now working as expected. `Labels` are now not tab stops by default.
- **Event Hit-Testing Fix:**
    - Fixed bug in EventManager::findTopObjectUnderMouse: the hidden-state check now uses the current node (`node.isHidden()`) instead of `rootNode`, and selection tie-breaks are deterministic.
    - Selection now uses a stable lexicographic rule (z-order primary, depth secondary, traversal sequence tertiary) so the topmost clickable object under the mouse is chosen reliably.

---
## [October 13, 2025]
- **Added inline numeric style escapes to Label tokenization:**
  - `[border=N]`, `[outline=N]`, `[pad=WxH]` / `[padding=WxH]`, `[dropshadow=X,Y]`
  - Parser accepts optional spaces (e.g. `[border = 3]`, `[border=    3]`) and single-number shorthand for padding.
  - Implemented per-field stacks so nested tags restore previous values correctly.
  - Defensive numeric parsing (validated before stoi) to avoid exceptions.

- **Tokenizer robustness and normalization:**
  - Centralized and normalized escape parsing (trimmed params, alias support for `pad`/`padding`).
  - Ensured numeric forms have precedence where appropriate while preserving existing color/escape semantics.
- **Color palette and mapping updates:**
  - Added `md_gray` (102) to complete the 6-step grayscale ramp.
  - Extended color map with many named colors (tan, orange, brown, gold, bronze, lime, mint, sea_green, royal_blue, etc.).
  - Documented named colors and hex escapes (`[RGB=rrggbb]`, `[RGBA=rrggbbaa]`) in label_text_parsing.md.
- **Tests:**
  - Implemented Label_test1() to validate internal resources created.
  - Implemented Label_test2() to validate SpriteSheet/BitmapFont asset existence and metrics
  - Implemented Label_test3() to validate SpriteSheet/BitmapFont asset existence and metrics (LUA)
  - Implemented Label_test4() to validate Label word/phrase style flags tokenization and inspection
  - Implemented Label_test5() to validate numeric-style escapes (border/outlines/padding/dropshadow) via Lua unit test.
  - Implemented Label_test6() to validate color-target escapes (`[fgnd=]`, `[bgnd=]`, `[border=]`, `[outline=]`, `[shadow=]`) using named and hex colors.
  - Built and ran the test suite; label-related tests (including new tests) passed locally.
  - **ToDo:**
    - Add unit tests for malformed/edge-case escapes (invalid hex, missing params, negative values).
- **IPanelObject**
  - `IPanelObject` has an interface and basic implementation.  Lua bindings are attached to `DisplayHandle`.
- **Docs and cleanup:**
  - Updated docs/label_text_parsing.md with numeric escape syntax, semantics, examples, and the full color list.
  - Removed debug prints and cleaned up temporary diagnostics used during development.

---
## [October 14, 2025]
- Added `Frame` scaffolding and registered the `Frame` DOM type with the Factory; SDOM_Frame.cpp implements minimal constructors and defers behavior to `IPanelObject`.
- `IPanelObject`:
  - Improved icon resource resolution (try configured name, then "<name>_SpriteSheet" fallback).
  - Removed temporary debug spam and added clearer, gated diagnostics.
- `SpriteSheet` loader: accepts snake_case keys (`sprite_width` / `sprite_height`) from Lua in addition to camelCase.
- Fonts & BitmapFont defaults:
  - Added `IFontObject::applyBitmapFontDefaults(...)` to populate missing `font_size`/`font_width`/`font_height` from a referenced `BitmapFont` asset.
  - Button/Group/Label now treat absent font metrics as unspecified and inherit BitmapFont metrics (e.g., external 8x12 fonts render at 8x12 without explicit metrics).
- `Button`:
  - Lua ctor applies `text` and font properties (`font_size`/`font_width`/`font_height`/`font_resource_name`), accepts common aliases, and preserves unspecified metrics for defaulting.
  - Exposed `getLabelObject()` to Lua via the `DisplayHandle` as the read-only `label` property.
  - Documented and validated `addEventListener` Lua usage (table form `{ type = EventType.X, listener = fn }`); example listeners switch root stages.
- `Group`:
  - Label creation uses font-type-aware glyph metrics (Bitmap sprite height or TrueType ascent) and centers the label over the top line of the group using the panel icon height.
- Lua config/tests:
  - External `SpriteSheet` resource creation and usage verified (`external_icon_8x8` resolves and is used as Frame icon).
  - Example button listeners wired to switch stages; both synthetic and real mouse events exercise listeners successfully.
- Misc:
  - Centralized keyboard focus pulsing rectangle in `Core` update/render logic.
  - Removed temporary diagnostics after verification; left gated INFO logs for troubleshooting.
  - Moved the various colors from `Label` to `IDisplayObject` as they are useful in places other than `Label`.
  - IconButton & Checkbox:
    - Added a lightweight `IconButton` display object that renders a single sprite index from a spritesheet and exposes IconIndex constants to Lua (IconIndex table + IconButton class constants).
    - IconIndex mapping centralized (enum ↔ name) and registered early in `Core` so Lua configs can reference `IconIndex.{index}` during startup.
    - `IconButton` provides safe accessors (getIconIndex/setIconIndex, getSpriteSheetHandle/getSpriteSheet) to avoid leaking internals.
    - `Checkbox` now composes an `IconButton` for the glyph; `setChecked()` updates the child icon index and queues a `StateChanged` event (payload includes previous/new state). Asset loading is handled by the Factory at creation so icons are ready before first render.

---
## [October 15, 2025]
- **Library Maintenance**
  - Cleaned up both system and local installs of the SDOM API library.
  - Removed deprecated headers and binaries from `/usr/include/SDOM`, `/usr/local/include/SDOM`, and $HOME..`'.local/include/SDOM`.
  - Rebuilt and reinstalled the library to ensure only current headers and binaries are present.
  - Fully removed the deprecated `TriStateCheckbox`, `Checkbox`, and `Radiobox` objects; they have been replaced by the new `TristateButton`, `CheckButton`, and `RadioButton` implementations.
- **UI controls:**
  - Added `CheckButton` as a descendant of `TristateButton`. `Checkbox` is now removed.
  - Added `RadioButton` as a descendant of `TristateButton`. `Radiobox` is now removed.
  - All button-like controls now use a unified state machine and expose consistent Lua APIs for state and interaction.
  - `CheckButton` and `RadioButton` accept multiple state aliases in Lua (e.g., `is_checked`, `checked`, `is_selected`, `selected`).
  - Exposed `text` property for `Button`, `RadioButton`, and `CheckButton` to Lua so label text can be read/modified from scripts.
  - **IButtonObject refactor and expansion:**
    - Expanded `IButtonObject` to better support future button objects (`IconButton`, `CheckButton`, `RadioButton`, `TristateButton`, `ArrowButton`, etc.).
    - Centralized logical state management using the `ButtonState` enum with canonical names and aliases (e.g., `Inactive`, `Active`, `Mixed`, `Disabled` and their synonyms).
    - Added mouse-hovered and key-focused flags to the interface for consistent UI state tracking.
    - Implemented static, idempotent `registerLuaBindings(sol::state_view lua)` method to expose `ButtonState` constants, conversion helpers, and DisplayHandle-level accessors (`getState`, `setState`, `isMouseHovered`, `setMouseHovered`, `isKeyFocused`, `setKeyFocused`) to Lua.
    - Ensured Lua bindings are reentrant and safe for multiple inheritance scenarios (e.g., CheckButton and RadioButton inheriting from TristateButton).
    - Updated derived controls to call `IButtonObject::registerLuaBindings(lua)` in their own Lua registration methods.
    - **Bug fix:** Resolved an issue in `TristateButton` where state updates were not reflected correctly. The control was using its own state variable instead of the base object's `buttonState_`. Now all state logic is unified and works as expected.
- **Fonts & labels:**
  - Finalized `IFontObject` improvements: `applyBitmapFontDefaults(...)` fills missing `font_size`/`font_width`/`font_height` from a referenced `BitmapFont` asset.
  - `Label` and label-creating parents (`Button`, `Group`) now detect font type (Bitmap vs Truetype), adopt the font's type/metrics, and preserve unspecified metrics so BitmapFont defaults apply correctly.
  - Added `label_color` parsing/aliases and thread-safe metric resolution so external 8x12 bitmap fonts render at 8x12 without explicit per-control metrics.
- **Group label centering:**
  - Fixed Group label vertical placement: compute glyph height by font type (Bitmap sprite height or TrueType ascent), obtain panel icon height via `getIconHeight()`, and position the label so its vertical center aligns with the top-line center inside the icon glyphs.
- **Button & Lua bindings:**
  - Button Lua ctor applies `text` and font properties (`font_size`/`font_width`/`font_height`/`font_resource` aliases).
  - Exposed `getLabelObject()` to Lua as read-only `label` on the `DisplayHandle`.
  - Documented and validated `addEventListener` Lua usage (table form `{ type = EventType.X, listener = fn }`); example listeners switch root stages.
- **EventType & Lua robustness:**
  - Hardened `EventType` Sol2 registration with a static guard so the usertype/metatable is always registered exactly once (avoids intermittent sol2 userdata/table mismatches).
  - Fixed Lua registration ordering issues that could cause intermittent unit-test failures.
- **Test harness & stability:**
  - Made `examples/test/repeat_tests.sh` robust: captures program output when logs are not saved, safely handles missing log files, and reliably prints failing run output.
  - Verified stability with long runs: repeated harness exercised (200 runs) with 0 failures locally.

---
## [October 16, 2025]
- **ArrowButton**
  - Added `ArrowButton` as a descendant of `IconButton`.
  - `ArrowButton` supports four directions (Up, Down, Left, Right) and two states (Active, Inactive).
  - Exposed `Direction` constants to Lua via `ArrowButton` class and `IconIndex` table.
  - Implemented safe accessors (`getDirection`/`setDirection`) and state management.
  - Added Lua bindings for `ArrowButton`, including constructors and property accessors.
- **IRangeControl**
  - Added `IRangeControl` interface to support range-based controls (ProgressBar, Slider, Scrollbar).
  - `IRangeControl` defines properties: `min`, `max`, `value`, `orientation` (horizontal/vertical), and events (`onValueChanged`).
  - Implemented static `registerLuaBindings(sol::state_view lua)` method to expose `Orientation` constants and accessors (`getMin`, `setMin`, `getMax`, `setMax`, `getValue`, `setValue`, `getOrientation`, `setOrientation`) to Lua.
  - `horizontal` and `vertical` orientation constants are available in Lua via `IRangeControl.Orientation`.
  - Ensured Lua bindings are reentrant and safe for multiple inheritance scenarios.
  - Derived controls should call `SUPER::registerLuaBindings(lua)` in their own Lua registration methods.
  - Added a new `IRangeControl_scaffold` to assist in creating new `RangeControl` objects
- **`Slider`:**
  - Added `Slider` scaffolding as a descendant of `IRangeControl`.
  - Implemented minimal constructors and deferred behavior to `IRangeControl`.
  - Registered `Slider` DOM type with the Factory.
  - Both `horizontal` and `vertical` orientations are now being rendered with the default `internal_icon_8x8` SpriteSheet.
  - Added a new property `step` for quantization of value changes when using a keyboard.
  - When selected, `PgUP` and `PgDn` change value by step * 10.0. If step is continuous (0.0f) it is treated as 1.0f.
  - Holding `SHIFT` while scrolling with the mouse wheel has a similar effect as `PgUP` and `PgDN`.
  - When selected, `HOME` and `END` set value to min or max as would be most relevant by orientation.
  - Slider knob and track are visually represented and mouse based interaction logic is functioning.
  - Slider emits `onValueChanged` events when the value changes.
  - Slider does expose `min`, `max`, `value`, and `orientation` properties to Lua.
- **`ProgressBar`:**
  - ProgressBar has been implimented as a read only range control.     
  - It supports both `vertical` and `horizontal` `Orientation`s.
  - `setValue()` has been exposed to C++ and Lua.
- **Defaults Forwarding:**
  - Implemented defaults-forwarding across the display/control constructors:
      Added/used defaults-aware base parsing (IDisplayObject(..., defaults) and IRangeControl(..., defaults)).
      Updated derived Lua ctors to forward their InitStruct() so derived defaults are visible during base parsing.
  - Controls updated (defaults forwarded): Slider, Box, Label, Frame, Group, Button, IconButton, CheckButton, RadioButton, TristateButton, ArrowButton, Stage, and IRangeControl_scaffold.
  - Removed local post-constructor color/Init fixes where forwarding made them redundant.
  - Added example/comment to SDOM_IRangeControl_scaffold showing the forwarding pattern and CreateFromLua example.

---
## [October 17, 2025]
- **ScrollBar:**
  - Added `ScrollBar` scaffolding as a descendant of `IRangeControl`.
  - Implemented minimal constructors and deferred behavior to `IRangeControl`.
  - Registered `ScrollBar` DOM type with the Factory.
  - Both `horizontal` and `vertical` orientations are now being rendered with the default `internal_icon_8x8` SpriteSheet.
  - Composes `ArrowButton`s for increment/decrement controls and a `ThumbTrack` with a draggable `Thumb`.
  - Supports both `vertical` and `horizontal` `Orientation`s.
  - **Now supports alternate icon sprite sizes:** ScrollBar can use any registered SpriteSheet (e.g., 8x8, 12x12, 16x16), and the thumb and track are positioned and scaled correctly for all sizes.
- **Slider:**
  - Added `Slider` scaffolding as a descendant of `IRangeControl`.
  - Registered `Slider` DOM type with the Factory.
  - Both `horizontal` and `vertical` orientations are now rendered with the default `internal_icon_8x8` SpriteSheet.
  - Added a `step` property for quantized value changes (keyboard and mouse wheel).
  - Slider knob and track are visually represented and mouse-based interaction logic is functioning.
  - Emits `onValueChanged` events when the value changes.
  - **Now supports alternate icon sprite sizes:** Slider can use any registered SpriteSheet (e.g., 8x8, 12x12, 16x16), and the knob and track are positioned and scaled correctly for all sizes.
  - **Knob padding:** Introduced `kKnobPaddingFraction` to reduce the active size of the knob, ensuring the visible part of the knob sprite aligns with the rail ends even if the sprite has transparent padding.
- **ProgressBar:**
  - Implemented as a read-only range control.
  - Supports both `vertical` and `horizontal` `Orientation`s.
  - `setValue()` is exposed to C++ and Lua.
  - **Now supports alternate icon sprite sizes:** ProgressBar can use any registered SpriteSheet (e.g., 8x8, 12x12, 16x16), and the bar and track are positioned and scaled correctly for all sizes.
- **IconButton and ArrowButton**
  - Both are now scaling to width and height.
  - `IconButton` now correctly uses the proper icon SpriteSheet,
  - The `border` flag doesnt seem to have any effect on `IconButton` based objects.
    - Check with `border_color` and `background_color` these should work with `IconButton` objects

---
## [October 18, 2025]
- **Universally support multiple icon set sizes:**
  - Refactored `IPanelObject` to properly render different sized icon SpriteSheet assets.
  - `Button`, `IconButton`, and `Frame` now make use of multi-sized `SpriteSheets` like the `RangeControls` now do.
  - `Checkbox` and `Radiobox` now make use of multi-sized `SpriteSheets` like the `RangeControls` now do.
- **UnitTest Refactoring:**
  - Refactored `SpriteSheet_UnitTests` to utilize proper scaffolding patterns.
  - Refactored `GarbageCollection_UnitTests` to utilize proper scaffolding patterns.
  - Refactored `Event_UnitTests` to utilize proper scaffolding patterns.
  - Refactored `EventType_UnitTests` to utilize proper scaffolding patterns.
  - Refactored `DisplayHandle_UnitTests` to utilize proper scaffolding patterns.
  - Refactored `Lua_UnitTests` to utilize proper scaffolding patterns.
- **Border and Background Flags:**
  - moved to `IDisplayObject` with accessors `getBorder()` and `getBackground()` as getters and `setBorder()` and s`etBackground()` as mutators,
  - Properties are likewise `border` and `background` in Lua.


---
### [October 19, 2025]

**- Implemented per-type Lua binding registry and dispatcher:**  
- Added `DisplayHandle::ensure_type_bind_table(...)` and a `SDOM_Bindings` registry.
  - `DisplayHandle` now routes lookups to per-type binding tables via a `__index` dispatcher.
- Migrated `Group::_registerLuaBindings` to register into the `Group` per-type table (demo migration).
- Completed per-item (per-type) binding migration for `Label`:
  - Moved `Label`-specific bindings into the `Label` per-type table.
  - Removed `Label` color properties from the handle surface and ensured `IDisplayObject` color accessors are authoritative.
  - Updated `Label` tokenization to consume `IDisplayObject` colors; `FontStyle` token tables continue to expose color data for tokens.
  - Ran examples/test unit suite — `Label`-related tests (`Label_test6`, `Label_test10`, `Label_test11`) now pass locally.
- Reverted temporary debug prints and cleaned up diagnostic logging introduced during investigation.
- Added a focused registration-order regression unit test (create + immediate method call) to catch userdata/binding order regressions; test added and executed locally.
- **Status**: `Group` and `Label` migrations complete; per-type binding dispatcher and fallback compatibility shims in place.
- **Next**: sweep remaining types (`Button`, `Frame`, `IconButton`, etc) to per-type tables, run full CI, and remove temporary backward-compatibility aliases where safe. All of the DisplayObjects will need to be swept to per-type tables and then fully tested with their own respective UnitTests.

---
### [October 20, 2025]
- Implemented a C++ per-type Lua binding registry (`LuaBindingRegistry`) and a small helper (`register_per_type`) to register protected functions reliably.
- Updated `DisplayHandle` dispatcher to consult the C++ registry first, then fall back to the existing SDOM_Bindings per-type table and the minimal handle. This preserves backward compatibility while enabling deterministic, per-type dispatch.
- Migrated `Label` and `Group` per-type bindings into the registry (also kept legacy table entries during transition) and adapted `IDisplayObject` bind helpers to use `register_per_type` where a per-type table exists.
- Diagnosed and fixed the `Label`/`Group` font getter/setter issues (forwarding wrappers + registry adapters); rebuilt and ran the full test suite — unit tests pass.
- Removed temporary debug prints added during investigation and replaced them with the plan to use gated debug macros if needed.
- Added diagnostic helpers and registry dump facilities for incremental verification.
- `Next steps:` sweep remaining display object types (`Button`, `Frame`, `IconButton`, etc.) to `register_per_type`, enable strict registry-only lookup behind a runtime toggle, and add a small set of targeted migration tests to catch ordering/regression issues.

- Now using SDOM_Core_LuaHelpers for registering Lua bindings in C++. 
  - We will be using this pattern going forward for all Lua binding registrations in C++.
  - _registerLuaBindings() will ultimately call into SDOM_Core_LuaHelpers to register the bindings to reduce excessive source file length.

---
### [October 21, 2025]
- **Major issues encountered with the dual Lua binding methodology**
  - Significant time spent attempting to maintain both per-item binding and fallback methods.
  - Decision made to refactor the Lua Binding System from the ground up:
    - Removed the old per-type table binding system.
    - Removed the old fallback system—no compatibility shims remain.
  - Began refactoring `Core` unit tests to use the new binding system. C++ tests are halfway completed.
    - Completed `Scaffolding` tests.
    - Completed `Window Dimensions` tests.
    - Completed `Pixel Dimensions` tests.
    - Completed `Aspect Ratio Preservation` tests.
    - Completed `Texture Resize Allowance` tests.
    - Completed `Pixel Format` tests.
    - Completed `Renderer Logical Presentation` tests.

---
### [October 22, 2025]
- **`Core_UnitTests` Refactor Progress:**
  - Continued refactoring `Core_UnitTests` to utilize the new Lua Binding System.
    - Completed `Window Flags` tests.
    - Completed `Core Background Color` tests.
    - Completed `Factory Existence` tests.
    - Completed `Callback/Hook Registration` tests.
    - Completed `Stage/Root Node Management` tests.
    
---
### [October 23, 2025]
- **`Core_UnitTests` Refactor Progress:**
  - Continued refactoring **Core_UnitTests** to utilize the new Lua Binding System.
    - Completed **SDL Resource Accessors** tests.
    - Completed **Configuration Getters/Setters** tests.
    - Completed **Factory and Event Manager Access** tests.
    - Completed **Focus & Hover Management** tests.
    - Completed **DisplayObject Creation** tests.
  - Constructed a new lua based unit test scaffolding 
    - Started **Core_UnitTests.lua** based on the new lua test scaffolding.
- **Lua Binding System stabilization and test fixes**
  - Idempotent usertype registration: updated **register_usertype_with_table** to register once per Lua state and reuse the existing usertype table (no re‑registration/clobbering).
  - Safe table creation: **ensure_sol_table** now reuses existing global entries (usertype/table) and avoids replacing them, preventing stale userdatas.
  - **DisplayHandle** minimal API + robust lookup: added **isValid**, **getName**, **getType**, **setName**, **setType**, and **get** with a resilient **__index** that supports both `:` and `.` calls.
  - Early/explicit registration: ensured **DisplayHandle** registration in **Core** and **Factory**, and before returning any handles from Core’s Lua helpers.
  - Handle return semantics: Core Lua wrappers now return **nil** (not an empty handle) when a requested object does not exist.
- **IDisplayObject** bindings (available via **DisplayHandle**)
  - Added generic hierarchy helpers: **addChild(childSpec)**, **removeChild(childSpec)**, **hasChild(childSpec)**, and **getChild(spec)** where specs accept a **DisplayHandle**, a name **string**, or **{ name = "..." }**`**.
  - Bound on both the **DisplayHandle** usertype and backing table for consistent lookup.
- **Lua tests and scaffolding**
  - **Core_UnitTests.lua** updated to verify **DisplayHandle** method availability, stage/root retrieval, invalid lookups returning **nil**, and basic parent/child operations.
  - Most Lua Integration checks are now passing after the binding fixes.
- **IDE stubs**
  - Expanded **lua/api_stubs.lua** for better autocompletion: **DisplayHandle** methods above, partial **IDisplayObject** surface (add/remove/has/get child), and partial **Stage** (mouse getters/setters).
- **Core Unit Test Results:**
  ```bash
    Starting [Core System] Unit Tests...
    [Core System] SDL_WasInit(SDL_INIT_VIDEO) [PASSED]
    [Core System] SDL Texture Validity [PASSED]
    [Core System] SDL Renderer Validity [PASSED]
    [Core System] SDL Window Validity [PASSED]
    [Core System] Tests [PASSED]
    Starting [Core] Unit Tests...
    [Core] Scaffolding [PASSED]
    [Core] Window Dimensions [PASSED]
    [Core] Pixel Dimensions [PASSED]
    [Core] Aspect Ratio Preservation [PASSED]
    [Core] Texture Resize Allowance [PASSED]
    [Core] Pixel Format [PASSED]
    [Core] Renderer Logical Presentation [PASSED]
    [Core] Window Flags [PASSED]
    [Core] Core Background Color [PASSED]
    [Core] Factory Existence [PASSED]
    [Core] Callback/Hook Registration [PASSED]
    [Core] Stage/Root Node Management [PASSED]
    [Core] SDL Resource Accessors [PASSED]
    [Core] Configuration Getters/Setters [PASSED]
    [Core] Factory and Event Manager Access [PASSED]
    [Core] Focus & Hover Management [PASSED]
    [Core] DisplayObject Creation [PASSED]
    [Core] Lua Integration [PASSED]
    [Core] Tests [PASSED]
  ```
---
### [October 24, 2025]
- **IDisplayObject_UnitTests** Progress:
  - Began building a new **IDisplayObject_UnitTests.cpp** Unit Test Module to fully test all C++ and LUA properties and functions.
  - Implemented **IDisplayObject** Lua wrappers via **SDOM_IDisplayObject_LuaHelpers.hpp**.
  - Added Unit Tests for the following **IDisplayObject** Lua bindings:
    - **Dirty/State Management**:
      - ✅ cleanAll()
      - ✅ getDirty()
      - ✅ setDirty()
      - ✅ isDirty()
    - **Debug/Utility**:
      - ✅ printTree()
    - **Events and Event Listener Handling**:
      - ✅ addEventListener(type, listener, useCapture, priority)
      - ✅ removeEventListener(type, listener, useCapture)
      - ✅ addEventListener_any(descriptor, maybe_listener, maybe_useCapture, maybe_priority)
      - ✅ removeEventListener_any(descriptor, maybe_listener, maybe_useCapture)
      - ✅ addEventListener_any_short(descriptor)
      - ✅ removeEventListener_any_short(descriptor)
      - ✅ hasEventListener(type, useCapture)
      - ✅ queue_event(type, init_payload)
    - **Hierarchy Management**:
      - ✅ addChild(childSpec)
      - ✅ getChild(spec)
      - ✅ removeChild(childSpec)
      - ✅ hasChild(childSpec)
      - ✅ getParent()
      - ✅ setParent(parentHandle)
      - ✅ isAncestorOf(descendantSpec)
      - ✅ isDescendantOf(ancestorSpec)
      - ✅ removeFromParent()
      - ✅ removeDescendant(descendantSpec)

  **1. Event Lifecycle is Now Proven Correct**
    - **Completed full event listener testing, including:**
      - Listener registration
      - Event queuing
      - Deferred dispatch via pumpEventsOnce()
      - Payload transfer
      - Listener removal
      - Re-check of listener presence
    - **This verifies:**
      - EventManager → Event dispatch → IDisplayObject listener flow works
      - Payloads survive through dispatch
      - Listeners are cleaned up correctly
      - No ghost callbacks / No leaks    
    - This was one of the most critical pieces of the SDOM runtime model — and it is now rock-solid. 

  **2. Dirty-State System is Verified**
    - The dirty-propagation model behaved exactly as expected:
      - Children do not dirty parents implicitly 
      - setDirty() and cleanAll() work recursively
      - cleanAll() does not prematurely delete anything
    - This confirms rendering invalidation strategy is correct and predictable.  

  **3. Hierarchy Management is Fully Validated**
    - The tests confirmed that:
      - Child add/remove works in both handle and name forms
      - Parent references update properly
      - isAncestorOf / isDescendantOf logic is correct
      - removeFromParent() works safely
      - removeDescendant() works by both handle and name
      - Object lifetime remains fully owned by the Factory
    - This is the backbone of the scene graph, and it's behaving exactly as a retained-mode UI should.   

  **4. Font & SDL_TTF Cleanup Bug Resolved**
    - Tracked down a tricky issue:
      - TTF_CloseFont() was being called twice
      - The real root cause was actually Lua closing before assets were destroyed
      - By removing manual lua_close(getLua()), the asset lifecycle now aligns correctly with shutdown

- **Valgrind Check:**
  - Ran Valgrind on the full test suite to check for memory leaks.
    ```
    LEAK SUMMARY:
        definitely lost: 0 bytes
        indirectly lost: 0 bytes
        possibly lost: 0 bytes
    ```
  - Confirmed by "stress testing" (repeated test runs) that no leaks or crashes occur over the course of 2,000 iterations while running all current tests.
  - No memory leaks in neither the operating system nor the GPU memory. No invalid accesses detected.
  - The shutdown path is clean and deterministic — and Valgrind confirms it.
  - No heap creep. No GPU memory creep. No event buildup. No stale pointers. No race conditions.

---
### [October 25, 2025]
- **IDisplayObject_UnitTests** Progress:
  - Added Unit Tests for the following **IDisplayObject** Lua bindings:
    - **Type & Property Access (test #7)**
      - ✅ std::string getName_lua()
      - ✅ void setName_lua(const std::string& newName)
      - ✅ std::string getType_lua()
      - ✅ Bounds getBounds_lua()
      - ✅ void setBounds_lua(const sol::object& bobj)
      - ✅ SDL_Color getColor_lua()
      - ✅ void setColor_lua(const sol::object& colorObj)
      - ✅ SDL_Color getForegroundColor_lua()
      - ✅ void setForegroundColor_lua(const sol::object& colorObj)
      - ✅ SDL_Color getBackgroundColor_lua()
      - ✅ void setBackgroundColor_lua(const sol::object& colorObj)
      - ✅ SDL_Color getBorderColor_lua()
      - ✅ void setBorderColor_lua(const sol::object& colorObj)
      - ✅ SDL_Color getOutlineColor_lua()
      - ✅ void setOutlineColor_lua(const sol::object& colorObj)
      - ✅ SDL_Color getDropshadowColor_lua()
      - ✅ void setDropshadowColor_lua(const sol::object& colorObj)
      - ✅ bool hasBorder_lua(const IDisplayObject* obj)
      - ✅ bool hasBackground_lua(const IDisplayObject* obj)
      - ✅ void setBorder_lua(IDisplayObject* obj, bool hasBorder)
      - ✅ void setBackground_lua(IDisplayObject* obj, bool hasBackground)
    - **Priority and Z-Order (test #8)**
      - ✅ std::vector<DisplayHandle>& getChildren(const IDisplayObject* obj)
      - ✅ int countChildren_lua(const IDisplayObject* obj)
      - ✅ int getMaxPriority_lua(const IDisplayObject* obj)
      - ✅ int getMinPriority_lua(const IDisplayObject* obj)
      - ✅ int getPriority_lua(const IDisplayObject* obj)
      - ✅ void setToHighestPriority_lua(IDisplayObject* obj)
      - ✅ void setToLowestPriority_lua(IDisplayObject* obj)
      - ✅ void setToHighestPriority_lua_any(IDisplayObject* obj, const sol::object& descriptor)
      - ✅ void setToLowestPriority_lua_any(IDisplayObject* obj, const sol::object& descriptor)
      - ✅ void sortChildrenByPriority_lua(IDisplayObject* obj)
      - ✅ void setPriority_lua(IDisplayObject* obj, int priority)
      - ✅ void setPriority_lua_any(IDisplayObject* obj, const sol::object& descriptor)
      - ✅ void setPriority_lua_target(IDisplayObject* obj, const sol::object& descriptor, int value)
      - ✅ std::vector<int> getChildrenPriorities_lua(const IDisplayObject* obj)
      - ✅ void moveToTop_lua(IDisplayObject* obj)
      - ✅ void moveToTop_lua_any(IDisplayObject* obj, const sol::object& descriptor)
      - ✅ void moveToBottom_lua(IDisplayObject* obj)
      - ✅ void moveToBottom_lua_any(IDisplayObject* obj, const sol::object& descriptor)
      - ✅ void bringToFront_lua(IDisplayObject* obj)
      - ✅ void bringToFront_lua_any(IDisplayObject* obj, const sol::object& descriptor)
      - ✅ void sendToBack_lua(IDisplayObject* obj)
      - ✅ void sendToBack_lua_any(IDisplayObject* obj, const sol::object& descriptor)
      - ✅ void sendToBackAfter_lua(IDisplayObject* obj, const IDisplayObject* limitObj)
      - ✅ void sendToBackAfter_lua_any(IDisplayObject* obj, const sol::object& descriptor, const IDisplayObject* limitObj)
      - ✅ int getZOrder_lua(const IDisplayObject* obj)
      - ✅ void setZOrder_lua(IDisplayObject* obj, int z_order)
      - ✅ void setZOrder_lua_any(IDisplayObject* obj, const sol::object& descriptor)
    - **Object Focus and Interactivity (test #9):**
      - ✅ void setKeyboardFocus_lua(IDisplayObject* obj)
      - ✅ bool isKeyboardFocused_lua(const IDisplayObject* obj)
      - ✅ bool isMouseHovered_lua(const IDisplayObject* obj)
      - ✅ bool isClickable_lua(const IDisplayObject* obj)
      - ✅ void setClickable_lua(IDisplayObject* obj, bool clickable)
      - ✅ bool isEnabled_lua(const IDisplayObject* obj)
      - ✅ void setEnabled_lua(IDisplayObject* obj, bool enabled)
      - ✅ bool isHidden_lua(const IDisplayObject* obj)
      - ✅ void setHidden_lua(IDisplayObject* obj, bool hidden)
      - ✅ bool isVisible_lua(const IDisplayObject* obj)
      - ✅ void setVisible_lua(IDisplayObject* obj, bool visible)    
  - **Supporting Improvements**
    - Added **SDL_Utils::color_equal()**, **color_not_equal()**, and **color_to_string()** for readable assertions.
    - Renamed **getBorder()** → **hasBorder()** and **getBackground()** → **hasBackground()** for clearer boolean intent.
    - Added **SDL_Utils::color_equal()**, **color_not_equal()**, and **color_to_string()** helpers for clean, readable test assertions.      
    - Renamed **getBorder()** to **hasBorder()** and **getBackground()** to **hasBackground()** for clarity and consistency with boolean semantics.
    - Implemented and fully tested 
      - **moveToBottom()**, **moveToTop()**, **bringToFront()**, **sendToBack()**, **sendToBackAfter()** (relative reordering)
      - All **Lua descriptor forms** (`*_lua_any`) now tested and confirmed working.
      - Z-order and priority maintain **stable + predictable ordering** across siblings.
      - No orphaned objects left behind across add/remove/reparent sequences.
    - **IDisplayObject_UnitTests** Output Summary:
    ```bash
      Starting [IDisplayObject] Unit Tests...
      [IDisplayObject] Scaffold [PASSED]
      [IDisplayObject] Create Stage Object [PASSED]
      [IDisplayObject] Get and Set Name [PASSED]
      [IDisplayObject] Destroy the generic Stage Object [PASSED]
      [IDisplayObject] Dirty/State Management [PASSED]
      [IDisplayObject] Events and Event Listener Handling [PASSED]
      [IDisplayObject] Hierarchy Management [PASSED]
      [IDisplayObject] Type and Property Access [PASSED]
      [IDisplayObject] Priority and Z-Order [PASSED]
      [IDisplayObject] Lua Integration [PASSED]
      [IDisplayObject] Tests [PASSED]
    ```
    - **valgrind --leak-check=full ./prog --stop_after_tests**
      ```
        ==136007== LEAK SUMMARY:
        ==136007==    definitely lost: 0 bytes in 0 blocks
        ==136007==    indirectly lost: 0 bytes in 0 blocks
        ==136007==      possibly lost: 0 bytes in 0 blocks
        ==136007==    still reachable: 287,117 bytes in 3,600 blocks
        ==136007==         suppressed: 0 bytes in 0 blocks

      ```

---
## Next Steps:
- Continue building out **IDisplayObject_UnitTests.cpp** Unit Test Module to fully test all C++ and LUA properties and functions.

---
## Long Term To Do:
- Sweep all DisplayObjects to use the new Lua Binding System.
- Update all Unit Tests to use the new Lua Binding System.
  - Achieve full C++ test coverage for each DisplayObject type.
  - As the final C++ test, run a complete Lua file that mirrors the C++ tests to ensure parity.

## Current DisplayObject Inheritance Hierarchy:
```
─── IDisplayObject
    ├── IPanelObject
    │   ├── Button
    │   ├── Frame
    │   └── Group
    ├── IRangeControl
    │   ├── IRangeControl_scaffold
    │   ├── ProgressBar
    │   ├── ScrollBar
    │   └── Slider
    ├── IconButton
    │   └── ArrowButton
    ├── Label
    ├── Stage
    └── TristateButton
        ├── CheckButton
        └── RadioButton
```

---        
## UnitTest Modules
        ✅ Tests Verified
        🔄 In Progress
        ☐ Pending
- ☐ ArrowButton
- ☐ AssetHandle
- ☐ BitmapFont
- ☐ Button
- ☐ CheckButton
- ☐ CLR
- ✅ Core
- ☐ DisplayHandle
- ☐ Event
- ☐ EventManager
- ☐ EventType
- ☐ Factory
- ☐ Frame
- ☐ Group
- ☐ IAssetObject
- ☐ IButtonObject
- ☐ IconButton
- ☐ IDataObject 
- 🔄 IDisplayObject
- ☐ IFontObject
- ☐ IPanelObject
- ☐ IRangeControl
- ☐ Label
- ☐ ProgressBar
- ☐ RadioButton
- ☐ ScrollBar
- ☐ Slider
- ☐ SpriteSheet
- ☐ Stage
- ☐ Texture
- ☐ TristateButton
- ☐ TruetypeFont
- ☐ TTFAsset