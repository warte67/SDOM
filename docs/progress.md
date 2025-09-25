# Project Description
This project is a C++23 application utilizing SDL3 for graphical rendering and event handling. The goal is to create a simple Document Object Model (DOM) API for SDL3, focusing on building a GUI interface for SDL3 developers to use in their games or other cross-platform applications.


## Scripting Flexibility:
`Lua` integration via `Sol2` will be entirely optional—developers can use as much or as little `Lua` as they wish, or none at all.
`SDOM` will continue to support direct C++ initialization and configuration.
As long as language bindings are available, any language can be used to leverage the SDOM API, making the framework flexible for a wide range of development styles and needs.

By leveraging a high-level scripting language, developers can iterate and prototype features quickly, while still relying on C++ for performance-critical components. This hybrid approach combines the agility of scripting with the efficiency of native code, making SDOM both developer-friendly and highly performant.


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

# Next Steps:
Our next focus will be on replacing JSON configuration and initialization with Lua scripts using Sol2. This transition will enable more flexible and powerful scripting for unit tests, object setup, dialog scripting, stage scene management, and rapid application prototyping—all within Lua. 

# ToDo:

- Plan to implement a garbage_collection() function to clean up stray display objects. For now, stray objects are destroyed on application termination.
- Expand event system tests to include drag-and-drop functionality, stacking, and anchoring for `Box` objects.
- Reintroduce `IResourceObject` into the `Factory` for resource management alongside `IDisplayObject`.
- Add unit tests for the revised `Factory` and event system to ensure stability and robustness.
- Move resource and pointer tests to `onUnitTest()` methods when available.
- Add comprehensive unit tests for Factory, resource_ptr, and error conditions.
- Language Hooks
    - Plan for hooks to support other languages (e.g., Python bindings).
    - Add placeholder comments or sections.
- Documentation
    - Add Doxygen comments for all public API elements.
    - Update progress.md with daily progress and ToDo items.
- Testing
    - Add unit tests for macros and exception handling. 