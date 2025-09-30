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
    [ "Architecture Overview", "md_architecture__overview.html", [
      [ "Runtime Flow (High Level)", "md_architecture__overview.html#autotoc_md2", null ]
    ] ],
    [ "Core Framework Design", "md_core__framework.html", [
      [ "Overview", "md_core__framework.html#autotoc_md4", null ],
      [ "Core Responsibilities", "md_core__framework.html#autotoc_md5", null ],
      [ "Core Initialization Sequence & Pre-Initialization Configuration", "md_core__framework.html#autotoc_md6", [
        [ "Required Pre-Initialization Parameters:", "md_core__framework.html#autotoc_md7", null ],
        [ "Initialization Flow:", "md_core__framework.html#autotoc_md8", null ],
        [ "Example Configuration API:", "md_core__framework.html#autotoc_md9", null ],
        [ "Required Pre-Initialization Parameters:", "md_core__framework.html#autotoc_md11", null ],
        [ "Alternative Initialization: JSON Configuration", "md_core__framework.html#autotoc_md12", [
          [ "Example API:", "md_core__framework.html#autotoc_md13", null ]
        ] ]
      ] ],
      [ "Primary Callbacks", "md_core__framework.html#autotoc_md15", null ],
      [ "Hook & Callback Registration", "md_core__framework.html#autotoc_md16", [
        [ "Example (C++)", "md_core__framework.html#autotoc_md17", null ],
        [ "Example (Python/Rust)", "md_core__framework.html#autotoc_md18", null ]
      ] ],
      [ "Global Accessor Functions", "md_core__framework.html#autotoc_md19", [
        [ "Example Usage (with global accessor)", "md_core__framework.html#autotoc_md20", null ]
      ] ],
      [ "Composition Diagram", "md_core__framework.html#autotoc_md21", null ],
      [ "Event Flow", "md_core__framework.html#autotoc_md22", null ],
      [ "Unit Tests", "md_core__framework.html#autotoc_md23", null ],
      [ "Stage", "md_core__framework.html#autotoc_md24", null ],
      [ "SDOM Comprehensive Architecture Diagram", "md_core__framework.html#autotoc_md25", null ],
      [ "SDOM Event System Integration", "md_core__framework.html#autotoc_md26", null ],
      [ "Additional Interfaces and Utilities", "md_core__framework.html#autotoc_md27", null ],
      [ "Extensibility & Singleton Pattern", "md_core__framework.html#autotoc_md28", null ],
      [ "EventListener Support", "md_core__framework.html#autotoc_md29", [
        [ "Example (C)", "md_core__framework.html#autotoc_md30", null ],
        [ "Benefits", "md_core__framework.html#autotoc_md31", null ]
      ] ],
      [ "Registering Custom Stage Objects", "md_core__framework.html#autotoc_md32", [
        [ "Example", "md_core__framework.html#autotoc_md33", null ]
      ] ],
      [ "Integration Methods Summary", "md_core__framework.html#autotoc_md34", [
        [ "Flexibility & Use Cases", "md_core__framework.html#autotoc_md35", null ]
      ] ],
      [ "Performance Considerations", "md_core__framework.html#autotoc_md36", null ],
      [ "Single Stage, Single Window Rationale", "md_core__framework.html#autotoc_md37", null ],
      [ "Overriding the Default Stage", "md_core__framework.html#autotoc_md38", null ],
      [ "Multiple Stages as Scenes", "md_core__framework.html#autotoc_md39", null ],
      [ "Window Resizing & Texture Resolution", "md_core__framework.html#autotoc_md40", null ],
      [ "Shared Display Texture for Stages", "md_core__framework.html#autotoc_md41", null ],
      [ "Stages as Game States or Modes", "md_core__framework.html#autotoc_md42", null ],
      [ "Changing the Active Stage", "md_core__framework.html#autotoc_md43", null ]
    ] ],
    [ "Display Object Anchoring System", "md_display__object__anchoring.html", [
      [ "Overview", "md_display__object__anchoring.html#autotoc_md53", null ],
      [ "Anchor Points", "md_display__object__anchoring.html#autotoc_md54", [
        [ "Anchor Naming Conventions", "md_display__object__anchoring.html#autotoc_md46", null ],
        [ "Edge-Based Layout", "md_display__object__anchoring.html#autotoc_md47", null ],
        [ "Position and Size Calculation", "md_display__object__anchoring.html#autotoc_md48", null ],
        [ "Anchor Accessors/Mutators", "md_display__object__anchoring.html#autotoc_md49", null ],
        [ "Changing Anchor Points", "md_display__object__anchoring.html#autotoc_md50", null ],
        [ "Drag-and-Drop and Reparenting", "md_display__object__anchoring.html#autotoc_md51", null ],
        [ "Summary", "md_display__object__anchoring.html#autotoc_md52", null ],
        [ "Reversed versions for user convenience", "md_display__object__anchoring.html#autotoc_md55", null ]
      ] ],
      [ "JSON Interface", "md_display__object__anchoring.html#autotoc_md56", null ],
      [ "Position and Size Calculation", "md_display__object__anchoring.html#autotoc_md57", null ],
      [ "Changing Anchor Points", "md_display__object__anchoring.html#autotoc_md58", null ],
      [ "Hierarchical Anchoring", "md_display__object__anchoring.html#autotoc_md59", null ],
      [ "Backward Compatibility", "md_display__object__anchoring.html#autotoc_md60", null ],
      [ "Benefits", "md_display__object__anchoring.html#autotoc_md61", null ],
      [ "Example Use Cases", "md_display__object__anchoring.html#autotoc_md62", null ],
      [ "Anchoring Diagrams", "md_display__object__anchoring.html#autotoc_md65", [
        [ "1) Parent Anchor Grid and Child Edge Anchors", "md_display__object__anchoring.html#autotoc_md66", null ],
        [ "2) Anchor Change Recalculation (No Visual Jump)", "md_display__object__anchoring.html#autotoc_md67", null ]
      ] ]
    ] ],
    [ "DOM Propagation Design", "md_dom__propagation.html", [
      [ "Overview", "md_dom__propagation.html#autotoc_md69", null ],
      [ "Event Propagation Diagram", "md_dom__propagation.html#autotoc_md71", null ],
      [ "Traversal Types", "md_dom__propagation.html#autotoc_md72", [
        [ "1. Core-based DOM Traversal (Type 1)", "md_dom__propagation.html#autotoc_md73", null ],
        [ "2. EventManager-based Event Traversal (Type 2)", "md_dom__propagation.html#autotoc_md75", null ],
        [ "3. Factory-based Global Traversal (Type 3)", "md_dom__propagation.html#autotoc_md77", null ],
        [ "4. World Coordinate Backpropagation Traversal (Type 4)", "md_dom__propagation.html#autotoc_md78", null ]
      ] ],
      [ "Traversal Safety and Modification", "md_dom__propagation.html#autotoc_md79", null ],
      [ "Architectural Separation", "md_dom__propagation.html#autotoc_md80", null ],
      [ "Next Steps", "md_dom__propagation.html#autotoc_md82", null ]
    ] ],
    [ "Modern COM-like Interface and Hook Design", "md_exposing__hooks.html", [
      [ "- Extensible for future language hooks (Python, etc.)", "md_exposing__hooks.html#autotoc_md87", null ],
      [ "Dynamic Extensibility Strategies", "md_exposing__hooks.html#autotoc_md88", [
        [ "Overview", "md_exposing__hooks.html#autotoc_md84", null ],
        [ "Key Concepts", "md_exposing__hooks.html#autotoc_md85", null ],
        [ "Benefits", "md_exposing__hooks.html#autotoc_md86", null ],
        [ "Example: Dynamic Hook Registration in C++", "md_exposing__hooks.html#autotoc_md89", null ],
        [ "Example: Extern \"C\" Entry Point", "md_exposing__hooks.html#autotoc_md90", null ],
        [ "Example: Lua Integration with Sol2", "md_exposing__hooks.html#autotoc_md91", null ],
        [ "Example: Plugin/Module System", "md_exposing__hooks.html#autotoc_md92", null ],
        [ "Example: Reflection and Metadata", "md_exposing__hooks.html#autotoc_md93", null ],
        [ "Example: Smart Pointer Resource Management", "md_exposing__hooks.html#autotoc_md94", null ],
        [ "1. Dynamic Hook Registration", "md_exposing__hooks.html#autotoc_md95", null ],
        [ "2. Extern \"C\" Entry Points for Registration", "md_exposing__hooks.html#autotoc_md96", null ],
        [ "3. Lua/Script Integration", "md_exposing__hooks.html#autotoc_md97", null ],
        [ "4. Reflection and Metadata", "md_exposing__hooks.html#autotoc_md98", null ],
        [ "5. Plugin/Module System", "md_exposing__hooks.html#autotoc_md99", null ],
        [ "6. Smart Pointer and Resource Management", "md_exposing__hooks.html#autotoc_md100", null ],
        [ "Next Steps", "md_exposing__hooks.html#autotoc_md102", null ]
      ] ]
    ] ],
    [ "Extensible Factory Design", "md_extensible__factory.html", [
      [ "Factory Overview (Diagram)", "md_extensible__factory.html#autotoc_md105", null ],
      [ "Resource Type Management", "md_extensible__factory.html#autotoc_md106", null ],
      [ "Requirements", "md_extensible__factory.html#autotoc_md107", null ],
      [ "Recommended Approach", "md_extensible__factory.html#autotoc_md108", [
        [ "1. Initializer Structs and Lua Scripts", "md_extensible__factory.html#autotoc_md109", null ],
        [ "3. Factory Create Methods", "md_extensible__factory.html#autotoc_md110", null ],
        [ "4. Usage", "md_extensible__factory.html#autotoc_md111", null ]
      ] ],
      [ "Benefits", "md_extensible__factory.html#autotoc_md112", null ],
      [ "Implementation Strategy", "md_extensible__factory.html#autotoc_md114", [
        [ "1. Initializer Structs", "md_extensible__factory.html#autotoc_md115", null ],
        [ "1a. Standardized Constructor Signatures", "md_extensible__factory.html#autotoc_md116", null ],
        [ "2. Private Constructors & Friend Factory", "md_extensible__factory.html#autotoc_md117", null ],
        [ "3. Factory Registration System", "md_extensible__factory.html#autotoc_md118", null ],
        [ "4. Generic Factory Create Method", "md_extensible__factory.html#autotoc_md119", null ],
        [ "5. Extensibility", "md_extensible__factory.html#autotoc_md120", null ],
        [ "6. Error Handling & Validation", "md_extensible__factory.html#autotoc_md121", null ],
        [ "7. Example Usage", "md_extensible__factory.html#autotoc_md122", null ]
      ] ],
      [ "Standardized Constructors for IDisplayObjects", "md_extensible__factory.html#autotoc_md124", null ],
      [ "Scripting and Configuration Choice", "md_extensible__factory.html#autotoc_md125", null ],
      [ "Resource Pointer (<tt>resource_ptr</tt>) Design", "md_extensible__factory.html#autotoc_md126", [
        [ "How It Works", "md_extensible__factory.html#autotoc_md127", null ],
        [ "Example Usage", "md_extensible__factory.html#autotoc_md128", null ],
        [ "Benefits", "md_extensible__factory.html#autotoc_md129", null ],
        [ "Implementation Outline", "md_extensible__factory.html#autotoc_md130", null ],
        [ "Summary", "md_extensible__factory.html#autotoc_md131", null ]
      ] ]
    ] ],
    [ "Object Handles in SDOM", "md_object__handles.html", [
      [ "Two Trees in the Garden", "md_object__handles.html#autotoc_md134", null ],
      [ "Overview", "md_object__handles.html#autotoc_md135", null ],
      [ "Handle Types", "md_object__handles.html#autotoc_md136", [
        [ "1. DomHandle", "md_object__handles.html#autotoc_md137", null ],
        [ "2. ResHandle", "md_object__handles.html#autotoc_md138", null ]
      ] ],
      [ "Design Rationale", "md_object__handles.html#autotoc_md139", null ],
      [ "Inheritance Warning: Avoid Diamond Inheritance", "md_object__handles.html#autotoc_md141", null ],
      [ "Example Usage", "md_object__handles.html#autotoc_md142", null ],
      [ "Migration Strategy", "md_object__handles.html#autotoc_md143", null ],
      [ "Best Practices", "md_object__handles.html#autotoc_md144", null ],
      [ "Optional: Handle Resolution Flow (reference)", "md_object__handles.html#autotoc_md146", null ]
    ] ],
    [ "Project Description and Development Progress", "md_progress.html", [
      [ "Scripting and Configuration", "md_progress.html#autotoc_md148", null ],
      [ "Progress Updates", "md_progress.html#autotoc_md149", null ],
      [ "Next Steps (garbage collection / orphan retention)", "md_progress.html#autotoc_md150", null ],
      [ "ToDo:", "md_progress.html#autotoc_md151", [
        [ "Current repo/test state (summary)", "md_progress.html#autotoc_md152", null ]
      ] ]
    ] ],
    [ "Scripting and Configuration (Lua & C++)", "md_scripting__and__configuration.html", [
      [ "Goals", "md_scripting__and__configuration.html#autotoc_md154", null ],
      [ "Modes of Use", "md_scripting__and__configuration.html#autotoc_md155", null ],
      [ "Integration Points", "md_scripting__and__configuration.html#autotoc_md156", null ],
      [ "Example: Creating with Lua", "md_scripting__and__configuration.html#autotoc_md157", null ],
      [ "Safety and Performance", "md_scripting__and__configuration.html#autotoc_md158", null ],
      [ "Source of Truth", "md_scripting__and__configuration.html#autotoc_md159", null ],
      [ "SDOM API considerations for Lua", "md_scripting__and__configuration.html#autotoc_md160", null ],
      [ "Next steps (Lua integration)", "md_scripting__and__configuration.html#autotoc_md161", null ]
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
"classSDOM_1_1Core.html#a44d83d9bd4c5112e723559322edce360",
"classSDOM_1_1Event.html#a20e348cd7abfbf6b0bdf233da251ca59",
"classSDOM_1_1EventType.html#a647ab7d38f981dca7b5b00d818e2412c",
"classSDOM_1_1IDisplayObject.html#a20ec88bd7b74e55076b20b37b6178079",
"classSDOM_1_1IResourceObject.html#a9e370ce55cd0f363220283d9680e5b84",
"classSDOM_1_1Stage.html#a7e479652966d8c971d871da8a32d4a2f",
"lua__Core_8hpp.html#a88db2d7ac9233ac4fd153d32e6cfa4f6",
"md_exposing__hooks.html#autotoc_md98",
"structSDOM_1_1Core_1_1CoreConfig.html#ae090ffc2d568c3d2fcec1f3d795ad24f"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';