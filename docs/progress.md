## Project Description
This project is a C++23 application utilizing SDL3 for graphical rendering and event handling. The goal is to create a simple Document Object Model (DOM) API for SDL3, focusing on building a GUI interface for SDL3 developers to use in their games or other cross-platform applications.

## Progress Updates

- **[July 2, 2025]** Initial rough draft and proof of concept.
- **[August 5, 2025]** SDOM secondary rough draft
- **[September 19, 2025]**
  - Proof of concept approved. Main development approved. Work begins officially.

  
# ToDo:

1. Refine SDOM.hpp Includes
    - Remove unnecessary standard and SDL includes.
    - Only include what’s needed for macros and exceptions.
    - Move heavy/optional includes to implementation files.
2. Macro Improvements
    - Ensure macros are robust and documented.
    - Add Doxygen comments for each macro.
3. Global Accessors
    - Decide on global accessor functions (e.g., for SDL objects).
    - Document intended usage and limitations.
4. Language Hooks
    - Plan for hooks to support other languages (e.g., Python bindings).
    - Add placeholder comments or sections.
5. Documentation
    - Add Doxygen comments for all public API elements.
    - Update progress.md with daily progress and ToDo items.
6. Testing
    - Add unit tests for macros and exception handling. 