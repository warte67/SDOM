# Project Description and Development Progress
SDOM is a C++23 library built on SDL3 that brings a DOM‑like scene graph and GUI toolkit to 2D applications and games. The aim is a small, predictable core for layout, events, and rendering so SDL developers can compose interfaces and in‑game UI with familiar patterns and a minimal API surface.

Architecturally, a single Core orchestrates the SDL runtime (window, renderer, main loop). A Stage serves as the root of the display tree, while an EventManager dispatches input through capture → target → bubble phases. The Factory registers, creates, and tracks resources and display objects by type and unique name, returning typed handles for safe references. Layout is driven by an edge‑based anchoring system that supports asymmetric, responsive designs.

Scripting and configuration are first‑class with Lua via Sol2 to enable data‑driven scenes, rapid iteration, and test automation. You can also build everything directly in C++—Lua is optional—but when enabled it can construct objects, wire scenes, drive dialog, and swap stages at runtime without recompilation.

The project emphasizes clear, living documentation. The Markdown design docs are working papers that evolve with the code. Expect them to change as features land and APIs are refined. Public APIs are defined in headers and surfaced via Doxygen, with correctness exercised by unit tests; if a doc briefly diverges from current behavior, treat the code as source‑of‑truth until the docs catch up. Diagrams are authored in Mermaid and exported to static SVG/PNG by the repo’s export pipeline, keeping visuals portable while ensuring the sources remain easy to edit. Progress updates will call out notable documentation changes, and contributions to clarify or correct docs are welcome.

This page is the hub for progress. The “Progress Updates” below record milestones chronologically with context and notes; the “Next Steps” and “ToDo” sections outline near‑term priorities and open tracks. Check back to see what landed and what’s coming next—and feel free to open issues or proposals as the API stabilizes.


## Scripting and Configuration
Lua (via Sol2) is first‑class but optional—you can script scenes and behavior in Lua, build everything in C++, or mix both. The Factory accepts `sol::table` configs alongside initializer structs, and Core’s lifecycle hooks make it easy to integrate scripts for iteration and tests. See the dedicated design doc for details and examples: [Scripting and Configuration (Lua + C++)](scripting_and_configuration.md).


## Progress Updates

- ### [July 2, 2025]  Initial rough draft and proof of concept.
- ### [August 5, 2025] SDOM secondary rough draft
- ### [September 19, 2025]
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

- ### [September 20, 2025]
    - Factory and resource_ptr scaffolding completed; code compiles and runs cleanly.
    - Event system foundation implemented and documented.
    - Doxygen documentation added for major interfaces (Core, Factory, IResourceObject).
    - Example usage of Core and Factory included in documentation.
    - Circular include and pointer management issues resolved (raw pointer for Factory ownership).
    - The SDOM framework now has a working Core singleton, Factory resource management, and resource_ptr smart handle. The codebase is stable, maintainable, and ready for expanded testing and feature development.

- ### [September 21, 2025]
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

- ### [September 22, 2025]
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

- ### [September 23, 2025]
    - **Event System Preliminary Testing:**  
        - Successfully integrated and tested the event system with live SDL3 events.
        - Verified that mouse, keyboard, window, and focus events are received and dispatched correctly.
        - Confirmed that custom display objects (Box) receive and handle events as expected.
        - Minimal "life tests" for event propagation and Box object interaction are passing.
        - Debug output confirms event flow from SDL3 through Core, EventManager, and into display objects.
        - Refactored event dispatch logic for single-stage architecture, simplifying code and improving maintainability.
        - Cleaned up circular includes and pointer conversion issues, improving build stability.
        - Next steps: Expand event tests for Box (drag/drop, stacking, anchoring), add more interactive unit tests, and document event system usage.
        - Refactored codebase to use new `DomHandle` and `ResHandle` types throughout.
        - Verified and debugged handle logic, ensuring safe pointer access and robust operator overloading.
        - Cleaned up event system: `EventManager` now fully leverages the handle system, removing unnecessary casts and improving safety.
        - Fixed segmentation faults and infinite recursion issues in tree traversal by clarifying handle usage and recursive logic.
        - Improved code readability and maintainability by removing legacy patterns and redundant checks.
        - All major event and display object interactions now use the new handle API.
        - Codebase is stable, with most reference and legacy issues resolved.

- ### [September 24, 2025]
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

- ### [September 25, 2025]
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

- ### [September 26, 2025]
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

- ### [September 27, 2025]
    - **Lua Integration Complete:**  
        - All major Core and Factory methods are now exposed to Lua via Sol2 usertype registration.
        - Overloaded methods and custom types (DomHandle, SDL_Color) are correctly bound and accessible from Lua scripts.
        - The Lua API is stable, with all required commands available for scripting, configuration, and test automation.
    - **Lua/C++ Unit Test Integration Enhanced:**  
        - Refined Lua-driven unit tests for display object creation, stage hierarchy, and destruction.
        - Confirmed that DomHandles automatically become invalid (`nullptr`) when their resource is destroyed in the Factory, ensuring safe reference semantics.
        - Improved `IDisplayObject::hasChild()` logic to check both handle presence and validity, preventing false positives when objects are deleted.
        - All current unit tests now pass, including tests for creation, addition to stage, and destruction/removal verification.
        - Added Lua-side debugging utilities: Factory registry and stage tree can be printed directly from Lua scripts for live inspection.
        - Demonstrated robust C++/Lua test integration: Lua scripts return boolean results to C++ test framework, enabling seamless cross-language validation.
        - Codebase and Lua API stability increased, with reliable resource management and hierarchy checks.

- ### [September 28, 2025]
    - **Lua Binding Improvements:**
        - Refined Lua usertype registration to ensure idempotent and consistent bindings.
        - Verified that all properties, commands, and functions are correctly exposed to Lua for Core, Factory, DomHandle, and IDisplayObject.
        - Added missing getters (`getX`, `getY`, `getWidth`, `getHeight`) to DomHandle Lua bindings.
        - Ensured that overloaded methods are properly handled in Lua.
        - Centralized Lua bindings: Factory now maintains a registry (`registerLuaProperty` / `registerLuaCommand` / `registerLuaFunction`) and applies entries idempotently in a two‑phase process (create minimal sol2 usertype, then attach registry entries).
        - Lua API & binding fixes: `DomHandle` and `Core` bindings improved (correct boxing/forwarding, colon-call signatures); EventType constants exposed; Sol2 runtime issues (nil-call/userdata mismatches) resolved.
        - Test helpers: added `Core:pushMouseEvent({x,y,type,button})` and `Core:pumpEventsOnce()` so Lua can synthesize and dispatch SDL events deterministically.
        - DomHandle Lua conveniences: `addEventListener` forwarding and simple getters (`getX`/`getY`/`getWidth`/`getHeight`) exposed to Lua.
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

- ### [September 29, 2025]
    - **Lua Binding Debugging:**
        - Added controlled debug prints to Lua usertype registration to trace binding operations.
        - Verified that each usertype (DomHandle, IDisplayObject, Stage, Factory, Core) is registered exactly once, preventing conflicts and ensuring consistent behavior.
        - Debug prints can be toggled with the `DEBUG_REGISTER_LUA` constant in `SDOM.hpp`.
    - **Binding Consistency:**
        - Ensured that all Lua bindings are idempotent and do not conflict with each other.
        - Verified that overloaded methods and custom types are correctly handled in Lua.
    - **Code Cleanup:**
        - Removed unnecessary debug prints from production code, leaving controlled logging for future debugging needs.
        - Ensured the codebase remains clean and maintainable.
    - **Core Lua Wrappers Verified:**
        - Completed verification of Core/Factory Lua wrapper coverage (get/set window title, event helpers, DomHandle forwards, focus/hover helpers, object lookup, and orphan management commands exposed to Lua).
        - Added `test20_lua` which validates `Core:getWindowTitle()` and `Core:setWindowTitle()` round-trip behavior; the `examples/test/prog` test binary builds and runs with the new test.
        - As requested, the `clearFactory` Lua binding was removed (it can be reintroduced later if needed).
        

# Next Steps (short term):
- Continue testing ALL properties, commands, and functions in the `Lua_UnitTests`
- Verify simple getters (`getX`/`getY`/`getWidth`/`getHeight`) are exposed correctly.
- Tidy informational registration prints (convert to controlled logging or remove) and document the canonical Lua-binding policy for contributors. Thouroughly clean up comments and orphaned functions. 
- Expand Lua examples and add more regression tests (invalid/nil handles, wrong-typed args, double-add/remove edge cases).

# ToDo:
- Expand Lua scripting examples and documentation.
- Continue improving resource management and garbage collection.
- Plan for future language hooks (e.g., Python).
- Maintain and update Doxygen and Markdown docs as features evolve.
- Implement garbage_collection() for display objects.
