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

  
# ToDo:

- Develop base framework classes:
  - `IDisplayObject`: Base interface for display objects. Handles anchoring and positioning relative to parent and child edges. Inherits from `IDataObject`.
  - `IResourceObject`: Interface for resource objects managed by the Factory. Inherits from `IDataObject`.
  - `Factory`: Owns and manages all resources and display objects. Responsible for their lifetime.
  - `Core`: Singleton that runs the main loop, handles callbacks, error logging, rendering, and event dispatch for the DOM tree.
  - `Stage`: Manages the SDL application window and its resources.

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