## Project Description
This project is a C++23 application utilizing SDL3 for graphical rendering and event handling. The goal is to create a simple Document Object Model (DOM) API for SDL3, focusing on building a GUI interface for SDL3 developers to use in their games or other cross-platform applications.


## Progress Updates

- **[July 2, 2025]** Initial rough draft and proof of concept.
- **[August 5, 2025]** SDOM secondary rough draft
- **[September 19, 2025]**
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

- **[September 20, 2025]**
    - Factory and resource_ptr scaffolding completed; code compiles and runs cleanly.
    - Event system foundation implemented and documented.
    - Doxygen documentation added for major interfaces (Core, Factory, IResourceObject).
    - Example usage of Core and Factory included in documentation.
    - Circular include and pointer management issues resolved (raw pointer for Factory ownership).
    - The SDOM framework now has a working Core singleton, Factory resource management, and resource_ptr smart handle. The codebase is stable, maintainable, and ready for expanded testing and feature development.
- **[September 21, 2025]**
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

# ToDo:
- Implement parent/child node relationships for resources and display objects.
- Move resource and pointer tests to `onUnitTest()` methods when available.
- Continue expanding Factory functionality (add/remove, type safety, extensibility).
- Add comprehensive unit tests for Factory, resource_ptr, and error conditions.
- Continue improving documentation and example usage.
- Develop base framework classes:
  - `IDisplayObject`: Base interface for display objects. Handles anchoring and positioning relative to parent and child edges. Inherits from `IDataObject`.
  - `IResourceObject`: Interface for resource objects managed by the Factory. Inherits from `IDataObject`.
  - `Factory`: Owns and manages all resources and display objects. Responsible for their lifetime.
    - Expand Factory implementation: add/remove resource methods, type safety, extensibility.
    - Add comprehensive unit tests for Factory and resource_ptr (including error conditions).
    - Plan and implement reference counting for resource_ptr (deferred until after initial testing).
    - Continue improving Doxygen documentation and example usage.
  - `Core`: Singleton that runs the main loop, handles callbacks, error logging, rendering, and event dispatch for the DOM tree.
  - `Stage`: Manages the SDL application window and its resources.

- Add unit tests for macros and exception handling. 

- Global Accessors
    - Decide on global accessor functions (e.g., for SDL objects).
    - Document intended usage and limitations.
- Language Hooks
    - Plan for hooks to support other languages (e.g., Python bindings).
    - Add placeholder comments or sections.
- Documentation
    - Add Doxygen comments for all public API elements.
    - Update progress.md with daily progress and ToDo items.
- Testing
    - Add unit tests for macros and exception handling. 