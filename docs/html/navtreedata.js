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
      [ "Runtime Flow (High Level)", "md_docs_2architecture__overview.html#autotoc_md3", null ]
    ] ],
    [ "Core Framework Design", "md_docs_2core__framework.html", [
      [ "Overview", "md_docs_2core__framework.html#autotoc_md5", null ],
      [ "Core Responsibilities", "md_docs_2core__framework.html#autotoc_md6", null ],
      [ "Core Initialization Sequence & Pre-Initialization Configuration", "md_docs_2core__framework.html#autotoc_md7", [
        [ "Required Pre-Initialization Parameters:", "md_docs_2core__framework.html#autotoc_md8", null ],
        [ "Initialization Flow:", "md_docs_2core__framework.html#autotoc_md9", null ],
        [ "Example Configuration API:", "md_docs_2core__framework.html#autotoc_md10", null ],
        [ "Alternative Initialization: Lua Configuration", "md_docs_2core__framework.html#autotoc_md12", null ]
      ] ],
      [ "Primary Callbacks", "md_docs_2core__framework.html#autotoc_md14", null ],
      [ "Hook & Callback Registration", "md_docs_2core__framework.html#autotoc_md15", [
        [ "Example (C++)", "md_docs_2core__framework.html#autotoc_md16", null ],
        [ "Example (Python/Rust)", "md_docs_2core__framework.html#autotoc_md17", null ]
      ] ],
      [ "Global Accessor Functions", "md_docs_2core__framework.html#autotoc_md18", [
        [ "Example Usage (with global accessor)", "md_docs_2core__framework.html#autotoc_md19", null ]
      ] ],
      [ "Composition Diagram", "md_docs_2core__framework.html#autotoc_md20", null ],
      [ "Event Flow", "md_docs_2core__framework.html#autotoc_md21", null ],
      [ "Unit Tests", "md_docs_2core__framework.html#autotoc_md22", null ],
      [ "Stage", "md_docs_2core__framework.html#autotoc_md23", null ],
      [ "SDOM Comprehensive Architecture Diagram", "md_docs_2core__framework.html#autotoc_md24", null ],
      [ "SDOM Event System Integration", "md_docs_2core__framework.html#autotoc_md25", null ],
      [ "Additional Interfaces and Utilities", "md_docs_2core__framework.html#autotoc_md26", null ],
      [ "Extensibility & Singleton Pattern", "md_docs_2core__framework.html#autotoc_md27", null ],
      [ "EventListener Support", "md_docs_2core__framework.html#autotoc_md28", [
        [ "Example (C)", "md_docs_2core__framework.html#autotoc_md29", null ],
        [ "Benefits", "md_docs_2core__framework.html#autotoc_md30", null ]
      ] ],
      [ "Registering Custom Stage Objects", "md_docs_2core__framework.html#autotoc_md31", [
        [ "Example", "md_docs_2core__framework.html#autotoc_md32", null ]
      ] ],
      [ "Integration Methods Summary", "md_docs_2core__framework.html#autotoc_md33", [
        [ "Flexibility & Use Cases", "md_docs_2core__framework.html#autotoc_md34", null ]
      ] ],
      [ "Performance Considerations", "md_docs_2core__framework.html#autotoc_md35", null ],
      [ "Single Stage, Single Window Rationale", "md_docs_2core__framework.html#autotoc_md36", null ],
      [ "Overriding the Default Stage", "md_docs_2core__framework.html#autotoc_md37", null ],
      [ "Multiple Stages as Scenes", "md_docs_2core__framework.html#autotoc_md38", null ],
      [ "Window Resizing & Texture Resolution", "md_docs_2core__framework.html#autotoc_md39", null ],
      [ "Shared Display Texture for Stages", "md_docs_2core__framework.html#autotoc_md40", null ],
      [ "Stages as Game States or Modes", "md_docs_2core__framework.html#autotoc_md41", null ],
      [ "Changing the Active Stage", "md_docs_2core__framework.html#autotoc_md42", null ]
    ] ],
    [ "Display Object Anchoring System", "md_docs_2display__object__anchoring.html", [
      [ "Overview", "md_docs_2display__object__anchoring.html#autotoc_md52", null ],
      [ "Anchor Points", "md_docs_2display__object__anchoring.html#autotoc_md53", [
        [ "Anchor Naming Conventions", "md_docs_2display__object__anchoring.html#autotoc_md45", null ],
        [ "Edge-Based Layout", "md_docs_2display__object__anchoring.html#autotoc_md46", null ],
        [ "Position and Size Calculation", "md_docs_2display__object__anchoring.html#autotoc_md47", null ],
        [ "Anchor Accessors/Mutators", "md_docs_2display__object__anchoring.html#autotoc_md48", null ],
        [ "Changing Anchor Points", "md_docs_2display__object__anchoring.html#autotoc_md49", null ],
        [ "Drag-and-Drop and Reparenting", "md_docs_2display__object__anchoring.html#autotoc_md50", null ],
        [ "Summary", "md_docs_2display__object__anchoring.html#autotoc_md51", null ],
        [ "Reversed versions for user convenience", "md_docs_2display__object__anchoring.html#autotoc_md54", null ]
      ] ],
      [ "Lua Interface", "md_docs_2display__object__anchoring.html#autotoc_md55", null ],
      [ "Position and Size Calculation", "md_docs_2display__object__anchoring.html#autotoc_md56", null ],
      [ "Changing Anchor Points", "md_docs_2display__object__anchoring.html#autotoc_md57", null ],
      [ "Hierarchical Anchoring", "md_docs_2display__object__anchoring.html#autotoc_md58", null ],
      [ "Backward Compatibility & Migration", "md_docs_2display__object__anchoring.html#autotoc_md59", null ],
      [ "Benefits", "md_docs_2display__object__anchoring.html#autotoc_md60", null ],
      [ "Example Use Cases", "md_docs_2display__object__anchoring.html#autotoc_md61", null ],
      [ "Anchoring Diagrams", "md_docs_2display__object__anchoring.html#autotoc_md64", [
        [ "1) Parent Anchor Grid and Child Edge Anchors", "md_docs_2display__object__anchoring.html#autotoc_md65", null ],
        [ "2) Anchor Change Recalculation (No Visual Jump)", "md_docs_2display__object__anchoring.html#autotoc_md66", null ]
      ] ],
      [ "Anchor helpers & parsing", "md_docs_2display__object__anchoring.html#autotoc_md67", null ],
      [ "Migration note", "md_docs_2display__object__anchoring.html#autotoc_md68", null ]
    ] ],
    [ "DOM Propagation Design", "md_docs_2dom__propagation.html", [
      [ "Overview", "md_docs_2dom__propagation.html#autotoc_md70", null ],
      [ "Event Propagation Diagram", "md_docs_2dom__propagation.html#autotoc_md72", null ],
      [ "Traversal Types", "md_docs_2dom__propagation.html#autotoc_md73", [
        [ "1. Core-based DOM Traversal (Type 1)", "md_docs_2dom__propagation.html#autotoc_md74", null ],
        [ "2. EventManager-based Event Traversal (Type 2)", "md_docs_2dom__propagation.html#autotoc_md76", null ],
        [ "3. Factory-based Global Traversal (Type 3)", "md_docs_2dom__propagation.html#autotoc_md78", null ],
        [ "4. World Coordinate Backpropagation Traversal (Type 4)", "md_docs_2dom__propagation.html#autotoc_md79", null ]
      ] ],
      [ "Traversal Safety and Modification", "md_docs_2dom__propagation.html#autotoc_md80", [
        [ "Deferred actions processing (pseudo)", "md_docs_2dom__propagation.html#autotoc_md81", null ],
        [ "Thread-safety notes", "md_docs_2dom__propagation.html#autotoc_md82", null ]
      ] ],
      [ "Architectural Separation", "md_docs_2dom__propagation.html#autotoc_md83", null ],
      [ "Migration note", "md_docs_2dom__propagation.html#autotoc_md84", null ],
      [ "Next Steps", "md_docs_2dom__propagation.html#autotoc_md86", null ]
    ] ],
    [ "Modern COM-like Interface and Hook Design", "md_docs_2exposing__hooks.html", [
      [ "- Extensible for future language hooks (Python, etc.)", "md_docs_2exposing__hooks.html#autotoc_md91", null ],
      [ "Dynamic Extensibility Strategies", "md_docs_2exposing__hooks.html#autotoc_md92", [
        [ "Overview", "md_docs_2exposing__hooks.html#autotoc_md88", null ],
        [ "Key Concepts", "md_docs_2exposing__hooks.html#autotoc_md89", null ],
        [ "Benefits", "md_docs_2exposing__hooks.html#autotoc_md90", null ],
        [ "Example: Dynamic Hook Registration in C++", "md_docs_2exposing__hooks.html#autotoc_md93", null ],
        [ "Example: Extern \"C\" Entry Point", "md_docs_2exposing__hooks.html#autotoc_md94", null ],
        [ "Example: Lua Integration with Sol2", "md_docs_2exposing__hooks.html#autotoc_md95", null ],
        [ "Example: Plugin/Module System", "md_docs_2exposing__hooks.html#autotoc_md96", null ],
        [ "Example: Reflection and Metadata", "md_docs_2exposing__hooks.html#autotoc_md97", null ],
        [ "Example: Smart Pointer Resource Management", "md_docs_2exposing__hooks.html#autotoc_md98", null ],
        [ "1. Dynamic Hook Registration", "md_docs_2exposing__hooks.html#autotoc_md99", null ],
        [ "2. Extern \"C\" Entry Points for Registration", "md_docs_2exposing__hooks.html#autotoc_md100", null ],
        [ "3. Lua/Script Integration", "md_docs_2exposing__hooks.html#autotoc_md101", null ],
        [ "4. Reflection and Metadata", "md_docs_2exposing__hooks.html#autotoc_md102", null ],
        [ "5. Plugin/Module System", "md_docs_2exposing__hooks.html#autotoc_md103", null ],
        [ "6. Smart Pointer and Resource Management", "md_docs_2exposing__hooks.html#autotoc_md104", null ],
        [ "Next Steps", "md_docs_2exposing__hooks.html#autotoc_md106", null ]
      ] ]
    ] ],
    [ "Extensible Factory Design", "md_docs_2extensible__factory.html", [
      [ "Factory Overview (Diagram)", "md_docs_2extensible__factory.html#autotoc_md109", null ],
      [ "Resource Type Management", "md_docs_2extensible__factory.html#autotoc_md110", null ],
      [ "Requirements", "md_docs_2extensible__factory.html#autotoc_md111", null ],
      [ "Recommended Approach", "md_docs_2extensible__factory.html#autotoc_md112", [
        [ "1. Initializer Structs and Lua Scripts", "md_docs_2extensible__factory.html#autotoc_md113", null ],
        [ "3. Factory Create Methods", "md_docs_2extensible__factory.html#autotoc_md114", null ],
        [ "4. Usage", "md_docs_2extensible__factory.html#autotoc_md115", null ]
      ] ],
      [ "Benefits", "md_docs_2extensible__factory.html#autotoc_md116", null ],
      [ "Implementation Strategy", "md_docs_2extensible__factory.html#autotoc_md118", [
        [ "1. Initializer Structs", "md_docs_2extensible__factory.html#autotoc_md119", null ],
        [ "1a. Standardized Constructor Signatures", "md_docs_2extensible__factory.html#autotoc_md120", null ],
        [ "2. Private Constructors & Friend Factory", "md_docs_2extensible__factory.html#autotoc_md121", null ],
        [ "3. Factory Registration System", "md_docs_2extensible__factory.html#autotoc_md122", null ],
        [ "4. Generic Factory Create Method", "md_docs_2extensible__factory.html#autotoc_md123", null ],
        [ "5. Extensibility", "md_docs_2extensible__factory.html#autotoc_md124", null ],
        [ "6. Error Handling & Validation", "md_docs_2extensible__factory.html#autotoc_md125", null ],
        [ "7. Example Usage", "md_docs_2extensible__factory.html#autotoc_md126", null ]
      ] ],
      [ "Standardized Constructors for IDisplayObjects", "md_docs_2extensible__factory.html#autotoc_md128", null ],
      [ "Scripting and Configuration Choice", "md_docs_2extensible__factory.html#autotoc_md129", null ],
      [ "Resource Pointer (<tt>resource_ptr</tt>) Design", "md_docs_2extensible__factory.html#autotoc_md130", [
        [ "How It Works", "md_docs_2extensible__factory.html#autotoc_md131", null ],
        [ "Example Usage", "md_docs_2extensible__factory.html#autotoc_md132", null ],
        [ "Benefits", "md_docs_2extensible__factory.html#autotoc_md133", null ],
        [ "Implementation Outline", "md_docs_2extensible__factory.html#autotoc_md134", null ],
        [ "Summary", "md_docs_2extensible__factory.html#autotoc_md135", null ]
      ] ]
    ] ],
    [ "Object Handles in SDOM", "md_docs_2object__handles.html", [
      [ "Two Trees in the Garden", "md_docs_2object__handles.html#autotoc_md139", null ],
      [ "Overview", "md_docs_2object__handles.html#autotoc_md140", null ],
      [ "Handle Types", "md_docs_2object__handles.html#autotoc_md141", [
        [ "1. DomHandle", "md_docs_2object__handles.html#autotoc_md142", null ],
        [ "2. ResHandle", "md_docs_2object__handles.html#autotoc_md143", null ]
      ] ],
      [ "Design Rationale", "md_docs_2object__handles.html#autotoc_md144", null ],
      [ "Inheritance Warning: Avoid Diamond Inheritance", "md_docs_2object__handles.html#autotoc_md146", null ],
      [ "Example Usage", "md_docs_2object__handles.html#autotoc_md147", null ],
      [ "Migration Strategy", "md_docs_2object__handles.html#autotoc_md148", null ],
      [ "Best Practices", "md_docs_2object__handles.html#autotoc_md149", null ],
      [ "Optional: Handle Resolution Flow (reference)", "md_docs_2object__handles.html#autotoc_md151", null ]
    ] ],
    [ "Project Description and Development Progress", "md_docs_2progress.html", [
      [ "Scripting and Configuration", "md_docs_2progress.html#autotoc_md153", null ],
      [ "Progress Updates", "md_docs_2progress.html#autotoc_md154", null ],
      [ "- Ran examples/test/prog: unit tests pass; observed expected runtime output for OnInit/OnUpdate/OnRender/OnEvent/OnQuit Lua handlers.", "md_docs_2progress.html#autotoc_md169", null ],
      [ "- Verified: rebuilt the examples and test binary after these changes (<tt>./compile</tt> in <tt>examples/test</tt>) — build completed and <tt>prog</tt> produced successfully.", "md_docs_2progress.html#autotoc_md170", null ],
      [ "Garbage Collection / Orphan Retention", "md_docs_2progress.html#autotoc_md174", null ],
      [ "ToDo:", "md_docs_2progress.html#autotoc_md175", [
        [ "Current repo/test state (summary)", "md_docs_2progress.html#autotoc_md176", null ]
      ] ]
    ] ],
    [ "Scripting and Configuration (Lua & C++)", "md_docs_2scripting__and__configuration.html", [
      [ "Goals", "md_docs_2scripting__and__configuration.html#autotoc_md178", null ],
      [ "Modes of Use", "md_docs_2scripting__and__configuration.html#autotoc_md179", null ],
      [ "Integration Points", "md_docs_2scripting__and__configuration.html#autotoc_md180", null ],
      [ "Example: Creating with Lua", "md_docs_2scripting__and__configuration.html#autotoc_md181", null ],
      [ "Safety and Performance", "md_docs_2scripting__and__configuration.html#autotoc_md182", null ],
      [ "Source of Truth", "md_docs_2scripting__and__configuration.html#autotoc_md183", null ],
      [ "SDOM API considerations for Lua", "md_docs_2scripting__and__configuration.html#autotoc_md184", null ],
      [ "Next steps (Lua integration)", "md_docs_2scripting__and__configuration.html#autotoc_md185", null ],
      [ "Module-style configuration and lifecycle registration (recommended)", "md_docs_2scripting__and__configuration.html#autotoc_md186", null ]
    ] ],
    [ "SDOM: Simple SDL Document Object Model API", "md_README.html", [
      [ "Overview", "md_README.html#autotoc_md188", null ],
      [ "Features", "md_README.html#autotoc_md189", null ],
      [ "IDataObject: Data-Driven Base Type", "md_README.html#autotoc_md190", [
        [ "Key Features", "md_README.html#autotoc_md191", null ]
      ] ],
      [ "Getting Started", "md_README.html#autotoc_md192", [
        [ "Prerequisites", "md_README.html#autotoc_md193", null ],
        [ "Building the SDOM Library", "md_README.html#autotoc_md194", null ],
        [ "Running the Example", "md_README.html#autotoc_md195", null ]
      ] ],
      [ "Project Structure", "md_README.html#autotoc_md196", null ],
      [ "SDL3 Build Dependencies", "md_README.html#autotoc_md197", null ],
      [ "Debian Build Dependencies", "md_README.html#autotoc_md198", null ],
      [ "Arch Build Dependencies", "md_README.html#autotoc_md199", null ],
      [ "Freetype on Debian:", "md_README.html#autotoc_md200", null ],
      [ "Building and Installing SDL3_mixer, SDL3_image, and SDL_ttf", "md_README.html#autotoc_md201", null ],
      [ "Build per SDL3:", "md_README.html#autotoc_md202", null ],
      [ "Build and install SDL3 libraries", "md_README.html#autotoc_md203", null ],
      [ "Lua and Sol2", "md_README.html#autotoc_md204", null ],
      [ "Documentation", "md_README.html#autotoc_md205", [
        [ "Exporting Mermaid Diagrams to PNG/SVG (for Doxygen)", "md_README.html#autotoc_md206", null ]
      ] ],
      [ "License", "md_README.html#autotoc_md207", null ],
      [ "Author", "md_README.html#autotoc_md208", null ]
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
"classSDOM_1_1Core.html#a318167ada37c279681c50133a0771373",
"classSDOM_1_1DisplayObject.html#a81690275008f3806f234eb91a8d65fcc",
"classSDOM_1_1Event.html#afdfcb10855505f48c77bec0d3f661faf",
"classSDOM_1_1Factory.html#acfdf8c26eb5993cfd5e4897125ff6994",
"classSDOM_1_1IDisplayObject.html#ac316c78060747eacf8cb4c6ab84d3855",
"classSDOM_1_1Stage.html#a853fddbff86054aae8abc808cf55d1b6",
"lua__BindHelpers_8hpp.html#afd81c75f9a6783cc79d74cb34cea8d69",
"lua__IDisplayObject_8hpp.html#aa6c544fd780ecb68a1b274f2a6590544",
"namespaceSDOM.html#a39719ae038a6b9422b565f94bbe4f256",
"structSDOM_1_1IDisplayObject_1_1InitStruct.html#ac3ecdea943f2635afac7ee1dd693dcc7"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';