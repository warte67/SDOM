/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "SDOM - Simple SDL Document Object Model", "index.html", [
    [ "SDOM Documentation", "index.html", null ],
    [ "Architecture Overview", "md_docs_2architecture__overview.html", [
      [ "Runtime Flow (High Level)", "md_docs_2architecture__overview.html#autotoc_md2", null ]
    ] ],
    [ "Core Framework Design", "md_docs_2core__framework.html", [
      [ "Overview", "md_docs_2core__framework.html#autotoc_md4", null ],
      [ "Core Responsibilities", "md_docs_2core__framework.html#autotoc_md5", null ],
      [ "Core Initialization Sequence & Pre-Initialization Configuration", "md_docs_2core__framework.html#autotoc_md6", [
        [ "Required Pre-Initialization Parameters:", "md_docs_2core__framework.html#autotoc_md7", null ],
        [ "Initialization Flow:", "md_docs_2core__framework.html#autotoc_md8", null ],
        [ "Example Configuration API:", "md_docs_2core__framework.html#autotoc_md9", null ],
        [ "Required Pre-Initialization Parameters:", "md_docs_2core__framework.html#autotoc_md11", null ],
        [ "Alternative Initialization: JSON Configuration", "md_docs_2core__framework.html#autotoc_md12", [
          [ "Example API:", "md_docs_2core__framework.html#autotoc_md13", null ]
        ] ]
      ] ],
      [ "Primary Callbacks", "md_docs_2core__framework.html#autotoc_md15", null ],
      [ "Hook & Callback Registration", "md_docs_2core__framework.html#autotoc_md16", [
        [ "Example (C++)", "md_docs_2core__framework.html#autotoc_md17", null ],
        [ "Example (Python/Rust)", "md_docs_2core__framework.html#autotoc_md18", null ]
      ] ],
      [ "Global Accessor Functions", "md_docs_2core__framework.html#autotoc_md19", [
        [ "Example Usage (with global accessor)", "md_docs_2core__framework.html#autotoc_md20", null ]
      ] ],
      [ "Composition Diagram", "md_docs_2core__framework.html#autotoc_md21", null ],
      [ "Event Flow", "md_docs_2core__framework.html#autotoc_md22", null ],
      [ "Unit Tests", "md_docs_2core__framework.html#autotoc_md23", null ],
      [ "Stage", "md_docs_2core__framework.html#autotoc_md24", null ],
      [ "SDOM Comprehensive Architecture Diagram", "md_docs_2core__framework.html#autotoc_md25", null ],
      [ "SDOM Event System Integration", "md_docs_2core__framework.html#autotoc_md26", null ],
      [ "Additional Interfaces and Utilities", "md_docs_2core__framework.html#autotoc_md27", null ],
      [ "Extensibility & Singleton Pattern", "md_docs_2core__framework.html#autotoc_md28", null ],
      [ "EventListener Support", "md_docs_2core__framework.html#autotoc_md29", [
        [ "Example (C)", "md_docs_2core__framework.html#autotoc_md30", null ],
        [ "Benefits", "md_docs_2core__framework.html#autotoc_md31", null ]
      ] ],
      [ "Registering Custom Stage Objects", "md_docs_2core__framework.html#autotoc_md32", [
        [ "Example", "md_docs_2core__framework.html#autotoc_md33", null ]
      ] ],
      [ "Integration Methods Summary", "md_docs_2core__framework.html#autotoc_md34", [
        [ "Flexibility & Use Cases", "md_docs_2core__framework.html#autotoc_md35", null ]
      ] ],
      [ "Performance Considerations", "md_docs_2core__framework.html#autotoc_md36", null ],
      [ "Single Stage, Single Window Rationale", "md_docs_2core__framework.html#autotoc_md37", null ],
      [ "Overriding the Default Stage", "md_docs_2core__framework.html#autotoc_md38", null ],
      [ "Multiple Stages as Scenes", "md_docs_2core__framework.html#autotoc_md39", null ],
      [ "Window Resizing & Texture Resolution", "md_docs_2core__framework.html#autotoc_md40", null ],
      [ "Shared Display Texture for Stages", "md_docs_2core__framework.html#autotoc_md41", null ],
      [ "Stages as Game States or Modes", "md_docs_2core__framework.html#autotoc_md42", null ],
      [ "Changing the Active Stage", "md_docs_2core__framework.html#autotoc_md43", null ]
    ] ],
    [ "Display Object Anchoring System", "md_docs_2display__object__anchoring.html", [
      [ "Overview", "md_docs_2display__object__anchoring.html#autotoc_md53", null ],
      [ "Anchor Points", "md_docs_2display__object__anchoring.html#autotoc_md54", [
        [ "Anchor Naming Conventions", "md_docs_2display__object__anchoring.html#autotoc_md46", null ],
        [ "Edge-Based Layout", "md_docs_2display__object__anchoring.html#autotoc_md47", null ],
        [ "Position and Size Calculation", "md_docs_2display__object__anchoring.html#autotoc_md48", null ],
        [ "Anchor Accessors/Mutators", "md_docs_2display__object__anchoring.html#autotoc_md49", null ],
        [ "Changing Anchor Points", "md_docs_2display__object__anchoring.html#autotoc_md50", null ],
        [ "Drag-and-Drop and Reparenting", "md_docs_2display__object__anchoring.html#autotoc_md51", null ],
        [ "Summary", "md_docs_2display__object__anchoring.html#autotoc_md52", null ],
        [ "Reversed versions for user convenience", "md_docs_2display__object__anchoring.html#autotoc_md55", null ]
      ] ],
      [ "JSON Interface", "md_docs_2display__object__anchoring.html#autotoc_md56", null ],
      [ "Position and Size Calculation", "md_docs_2display__object__anchoring.html#autotoc_md57", null ],
      [ "Changing Anchor Points", "md_docs_2display__object__anchoring.html#autotoc_md58", null ],
      [ "Hierarchical Anchoring", "md_docs_2display__object__anchoring.html#autotoc_md59", null ],
      [ "Backward Compatibility", "md_docs_2display__object__anchoring.html#autotoc_md60", null ],
      [ "Benefits", "md_docs_2display__object__anchoring.html#autotoc_md61", null ],
      [ "Example Use Cases", "md_docs_2display__object__anchoring.html#autotoc_md62", null ],
      [ "Anchoring Diagrams", "md_docs_2display__object__anchoring.html#autotoc_md65", [
        [ "1) Parent Anchor Grid and Child Edge Anchors", "md_docs_2display__object__anchoring.html#autotoc_md66", null ],
        [ "2) Anchor Change Recalculation (No Visual Jump)", "md_docs_2display__object__anchoring.html#autotoc_md67", null ]
      ] ]
    ] ],
    [ "DOM Propagation Design", "md_docs_2dom__propagation.html", [
      [ "Overview", "md_docs_2dom__propagation.html#autotoc_md69", null ],
      [ "Event Propagation Diagram", "md_docs_2dom__propagation.html#autotoc_md71", null ],
      [ "Traversal Types", "md_docs_2dom__propagation.html#autotoc_md72", [
        [ "1. Core-based DOM Traversal (Type 1)", "md_docs_2dom__propagation.html#autotoc_md73", null ],
        [ "2. EventManager-based Event Traversal (Type 2)", "md_docs_2dom__propagation.html#autotoc_md75", null ],
        [ "3. Factory-based Global Traversal (Type 3)", "md_docs_2dom__propagation.html#autotoc_md77", null ],
        [ "4. World Coordinate Backpropagation Traversal (Type 4)", "md_docs_2dom__propagation.html#autotoc_md78", null ]
      ] ],
      [ "Traversal Safety and Modification", "md_docs_2dom__propagation.html#autotoc_md79", null ],
      [ "Architectural Separation", "md_docs_2dom__propagation.html#autotoc_md80", null ],
      [ "Next Steps", "md_docs_2dom__propagation.html#autotoc_md82", null ]
    ] ],
    [ "Modern COM-like Interface and Hook Design", "md_docs_2exposing__hooks.html", [
      [ "- Extensible for future language hooks (Python, etc.)", "md_docs_2exposing__hooks.html#autotoc_md87", null ],
      [ "Dynamic Extensibility Strategies", "md_docs_2exposing__hooks.html#autotoc_md88", [
        [ "Overview", "md_docs_2exposing__hooks.html#autotoc_md84", null ],
        [ "Key Concepts", "md_docs_2exposing__hooks.html#autotoc_md85", null ],
        [ "Benefits", "md_docs_2exposing__hooks.html#autotoc_md86", null ],
        [ "Example: Dynamic Hook Registration in C++", "md_docs_2exposing__hooks.html#autotoc_md89", null ],
        [ "Example: Extern \"C\" Entry Point", "md_docs_2exposing__hooks.html#autotoc_md90", null ],
        [ "Example: Lua Integration with Sol2", "md_docs_2exposing__hooks.html#autotoc_md91", null ],
        [ "Example: Plugin/Module System", "md_docs_2exposing__hooks.html#autotoc_md92", null ],
        [ "Example: Reflection and Metadata", "md_docs_2exposing__hooks.html#autotoc_md93", null ],
        [ "Example: Smart Pointer Resource Management", "md_docs_2exposing__hooks.html#autotoc_md94", null ],
        [ "1. Dynamic Hook Registration", "md_docs_2exposing__hooks.html#autotoc_md95", null ],
        [ "2. Extern \"C\" Entry Points for Registration", "md_docs_2exposing__hooks.html#autotoc_md96", null ],
        [ "3. Lua/Script Integration", "md_docs_2exposing__hooks.html#autotoc_md97", null ],
        [ "4. Reflection and Metadata", "md_docs_2exposing__hooks.html#autotoc_md98", null ],
        [ "5. Plugin/Module System", "md_docs_2exposing__hooks.html#autotoc_md99", null ],
        [ "6. Smart Pointer and Resource Management", "md_docs_2exposing__hooks.html#autotoc_md100", null ],
        [ "Next Steps", "md_docs_2exposing__hooks.html#autotoc_md102", null ]
      ] ]
    ] ],
    [ "Extensible Factory Design", "md_docs_2extensible__factory.html", [
      [ "Factory Overview (Diagram)", "md_docs_2extensible__factory.html#autotoc_md105", null ],
      [ "Resource Type Management", "md_docs_2extensible__factory.html#autotoc_md106", null ],
      [ "Requirements", "md_docs_2extensible__factory.html#autotoc_md107", null ],
      [ "Recommended Approach", "md_docs_2extensible__factory.html#autotoc_md108", [
        [ "1. Initializer Structs and Lua Scripts", "md_docs_2extensible__factory.html#autotoc_md109", null ],
        [ "3. Factory Create Methods", "md_docs_2extensible__factory.html#autotoc_md110", null ],
        [ "4. Usage", "md_docs_2extensible__factory.html#autotoc_md111", null ]
      ] ],
      [ "Benefits", "md_docs_2extensible__factory.html#autotoc_md112", null ],
      [ "Implementation Strategy", "md_docs_2extensible__factory.html#autotoc_md114", [
        [ "1. Initializer Structs", "md_docs_2extensible__factory.html#autotoc_md115", null ],
        [ "1a. Standardized Constructor Signatures", "md_docs_2extensible__factory.html#autotoc_md116", null ],
        [ "2. Private Constructors & Friend Factory", "md_docs_2extensible__factory.html#autotoc_md117", null ],
        [ "3. Factory Registration System", "md_docs_2extensible__factory.html#autotoc_md118", null ],
        [ "4. Generic Factory Create Method", "md_docs_2extensible__factory.html#autotoc_md119", null ],
        [ "5. Extensibility", "md_docs_2extensible__factory.html#autotoc_md120", null ],
        [ "6. Error Handling & Validation", "md_docs_2extensible__factory.html#autotoc_md121", null ],
        [ "7. Example Usage", "md_docs_2extensible__factory.html#autotoc_md122", null ]
      ] ],
      [ "Standardized Constructors for IDisplayObjects", "md_docs_2extensible__factory.html#autotoc_md124", null ],
      [ "Scripting and Configuration Choice", "md_docs_2extensible__factory.html#autotoc_md125", null ],
      [ "Resource Pointer (<tt>resource_ptr</tt>) Design", "md_docs_2extensible__factory.html#autotoc_md126", [
        [ "How It Works", "md_docs_2extensible__factory.html#autotoc_md127", null ],
        [ "Example Usage", "md_docs_2extensible__factory.html#autotoc_md128", null ],
        [ "Benefits", "md_docs_2extensible__factory.html#autotoc_md129", null ],
        [ "Implementation Outline", "md_docs_2extensible__factory.html#autotoc_md130", null ],
        [ "Summary", "md_docs_2extensible__factory.html#autotoc_md131", null ]
      ] ]
    ] ],
    [ "Object Handles in SDOM", "md_docs_2object__handles.html", [
      [ "Two Trees in the Garden", "md_docs_2object__handles.html#autotoc_md136", null ],
      [ "Overview", "md_docs_2object__handles.html#autotoc_md137", null ],
      [ "Handle Types", "md_docs_2object__handles.html#autotoc_md138", [
        [ "1. DomHandle", "md_docs_2object__handles.html#autotoc_md139", null ],
        [ "2. ResHandle", "md_docs_2object__handles.html#autotoc_md140", null ]
      ] ],
      [ "Design Rationale", "md_docs_2object__handles.html#autotoc_md141", null ],
      [ "Inheritance Warning: Avoid Diamond Inheritance", "md_docs_2object__handles.html#autotoc_md143", null ],
      [ "Example Usage", "md_docs_2object__handles.html#autotoc_md144", null ],
      [ "Migration Strategy", "md_docs_2object__handles.html#autotoc_md145", null ],
      [ "Best Practices", "md_docs_2object__handles.html#autotoc_md146", null ],
      [ "Optional: Handle Resolution Flow (reference)", "md_docs_2object__handles.html#autotoc_md148", null ]
    ] ],
    [ "Project Description and Development Progress", "md_docs_2progress.html", [
      [ "Scripting and Configuration", "md_docs_2progress.html#autotoc_md150", null ],
      [ "Progress Updates", "md_docs_2progress.html#autotoc_md151", null ],
      [ "Next Steps (garbage collection / orphan retention)", "md_docs_2progress.html#autotoc_md152", null ],
      [ "ToDo:", "md_docs_2progress.html#autotoc_md153", [
        [ "Current repo/test state (summary)", "md_docs_2progress.html#autotoc_md154", null ]
      ] ]
    ] ],
    [ "Scripting and Configuration (Lua & C++)", "md_docs_2scripting__and__configuration.html", [
      [ "Goals", "md_docs_2scripting__and__configuration.html#autotoc_md156", null ],
      [ "Modes of Use", "md_docs_2scripting__and__configuration.html#autotoc_md157", null ],
      [ "Integration Points", "md_docs_2scripting__and__configuration.html#autotoc_md158", null ],
      [ "Example: Creating with Lua", "md_docs_2scripting__and__configuration.html#autotoc_md159", null ],
      [ "Safety and Performance", "md_docs_2scripting__and__configuration.html#autotoc_md160", null ],
      [ "Source of Truth", "md_docs_2scripting__and__configuration.html#autotoc_md161", null ],
      [ "SDOM API considerations for Lua", "md_docs_2scripting__and__configuration.html#autotoc_md162", null ],
      [ "Next steps (Lua integration)", "md_docs_2scripting__and__configuration.html#autotoc_md163", null ]
    ] ],
    [ "SDOM: Simple SDL Document Object Model API", "md_README.html", [
      [ "Overview", "md_README.html#autotoc_md165", null ],
      [ "Features", "md_README.html#autotoc_md166", null ],
      [ "IDataObject: Data-Driven Base Type", "md_README.html#autotoc_md167", [
        [ "Key Features", "md_README.html#autotoc_md168", null ]
      ] ],
      [ "Getting Started", "md_README.html#autotoc_md169", [
        [ "Prerequisites", "md_README.html#autotoc_md170", null ],
        [ "Building the SDOM Library", "md_README.html#autotoc_md171", null ],
        [ "Running the Example", "md_README.html#autotoc_md172", null ]
      ] ],
      [ "Project Structure", "md_README.html#autotoc_md173", null ],
      [ "SDL3 Build Dependencies", "md_README.html#autotoc_md174", null ],
      [ "Debian Build Dependencies", "md_README.html#autotoc_md175", null ],
      [ "Arch Build Dependencies", "md_README.html#autotoc_md176", null ],
      [ "Freetype on Debian:", "md_README.html#autotoc_md177", null ],
      [ "Building and Installing SDL3_mixer, SDL3_image, and SDL_ttf", "md_README.html#autotoc_md178", null ],
      [ "Build per SDL3:", "md_README.html#autotoc_md179", null ],
      [ "Build and install SDL3 libraries", "md_README.html#autotoc_md180", null ],
      [ "Lua and Sol2", "md_README.html#autotoc_md181", null ],
      [ "Documentation", "md_README.html#autotoc_md182", [
        [ "Exporting Mermaid Diagrams to PNG/SVG (for Doxygen)", "md_README.html#autotoc_md183", null ]
      ] ],
      [ "License", "md_README.html#autotoc_md184", null ],
      [ "Author", "md_README.html#autotoc_md185", null ]
    ] ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", null ],
        [ "Functions", "namespacemembers_func.html", null ],
        [ "Variables", "namespacemembers_vars.html", null ],
        [ "Enumerations", "namespacemembers_enum.html", null ]
      ] ]
    ] ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Hierarchy", "hierarchy.html", "hierarchy" ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", "functions_dup" ],
        [ "Functions", "functions_func.html", "functions_func" ],
        [ "Variables", "functions_vars.html", null ],
        [ "Typedefs", "functions_type.html", null ],
        [ "Enumerations", "functions_enum.html", null ],
        [ "Related Symbols", "functions_rela.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "File Members", "globals.html", [
        [ "All", "globals.html", null ],
        [ "Variables", "globals_vars.html", null ],
        [ "Macros", "globals_defs.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"SDOM_8cpp.html",
"classSDOM_1_1Core.html#a42a5582607f3fe1540ed6c00afc40283",
"classSDOM_1_1Event.html#a1ef4b8a04e0d7a9cd0cc9fbee6e98a62",
"classSDOM_1_1EventType.html#a6311cdc9538efcbb20839869ec1b283c",
"classSDOM_1_1IDisplayObject.html#a1eae3efeba332531e97d56d9cf638cb5",
"classSDOM_1_1IResourceObject.html#a9a0e8c7c95a1a60291038e9da23b272b",
"classSDOM_1_1Stage.html#a78c24fdc177769b1a7604edaac795117",
"lua__Core_8hpp.html#a4ab0ffe141e146ee571570135c515a1d",
"md_docs_2dom__propagation.html#autotoc_md71",
"structSDOM_1_1Bounds.html#a94c040502f6cc3d683a928756708db4d"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';